#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from scapy.all import sniff, IP, TCP, Raw
import smtplib
from email.mime.text import MIMEText
from email.header import Header
import datetime
import traceback
import time

# ======================
# 配置
# ======================
INTERFACE = "enp0s8"
RULE_FILE = "/home/vboxuser/community-rules/community.rules"

SMTP_SERVER = "smtp.qq.com"
SMTP_PORT = 587
EMAIL_USER = "1561802244@qq.com"
EMAIL_PASS = "bjeoobzgoxcwfhhe"
EMAIL_TO = "1561802244@qq.com"

# 登录尝试等待窗口（秒），用于避免尝试和成功告警冲突
LOGIN_GRACE_PERIOD = 20

# ======================
# Telnet 会话状态表
# ======================
sessions = {}

# ======================
# 日志输出工具
# ======================
def log(level, msg):
    ts = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"[{ts}] [{level}] {msg}")

# ======================
# 邮件发送
# ======================
def send_email_alert(subject, content):
    log("MAIL", f"准备发送邮件 | Subject='{subject}'")

    try:
        msg = MIMEText(content, "plain", "utf-8")
        msg["From"] = f"Telnet-NIDS <{EMAIL_USER}>"
        msg["To"] = EMAIL_TO
        msg["Subject"] = Header(subject, "utf-8")
    except Exception:
        log("ERROR", "构造邮件内容失败")
        traceback.print_exc()
        return

    try:
        server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT, timeout=10)
        server.starttls()
        server.login(EMAIL_USER, EMAIL_PASS)
        server.sendmail(EMAIL_USER, EMAIL_TO, msg.as_string())
        log("MAIL", "邮件发送成功")
    except Exception:
        log("ERROR", "SMTP 发送邮件失败")
        traceback.print_exc()
    finally:
        try:
            server.quit()
        except:
            pass

# ======================
# 检查登录尝试超时（避免尝试和成功告警冲突）
# ======================
def check_login_timeout():
    now = time.time()
    for key, s in sessions.items():
        if s["stage"] == "LOGIN_ATTEMPT" and not s["attempt_alerted"]:
            if now - s.get("login_time", now) >= LOGIN_GRACE_PERIOD:
                s["attempt_alerted"] = True
                log("ALERT", f"Telnet 登录尝试超时告警 {key}")
                alert_msg = f"""
【Telnet 登录尝试告警】

时间: {datetime.datetime.now()}
源 IP: {key[0]}
源端口: {key[1]}
目标端口: 23
"""
                send_email_alert("！警报：有来自计算机外部的root登录尝试", alert_msg)

# ======================
# 数据包处理
# ======================
def packet_handler(pkt):
    if not (pkt.haslayer(IP) and pkt.haslayer(TCP) and pkt.haslayer(Raw)):
        return

    ip = pkt[IP]
    tcp = pkt[TCP]

    if tcp.sport != 23 and tcp.dport != 23: 
        return

    try:
        payload = pkt[Raw].load.decode(errors="ignore")
    except:
        return

    session_key = (ip.src, tcp.sport)

    if session_key not in sessions:
        sessions[session_key] = {
            "stage": "INIT",
            "attempt_alerted": False,
            "success_alerted": False,
            "login_time": None
        }
        log("SESSION", f"新 Telnet 会话 {session_key}")

    session = sessions[session_key]
    stage = session["stage"]
    payload_l = payload.lower()

    log("PKT", f"{ip.src}:{tcp.sport} -> {ip.dst}:23 | stage={stage} | payload={repr(payload[:60])}")

    # ======================
    # root 登录成功（优先级最高）
    # ======================
    if stage in ("INIT", "LOGIN_ATTEMPT", "ROOT", "PASS") and not session["success_alerted"]:
        if "root@" in payload or "# " in payload or "welcome" in payload_l or "last login" in payload_l:
            session["success_alerted"] = True
            session["stage"] = "DONE"
            log("ALERT", f"root 用户 Telnet 登录成功 {session_key}")
            alert_msg = f"""
【来自外部主机的Telnet root 用户登录成功警报】

时间: {datetime.datetime.now()}
源 IP: {ip.src}
目标 IP: {ip.dst}
源端口: {tcp.sport}
目标端口: 23

Payload:
{repr(payload)}
"""
            send_email_alert("🚨 有来自计算机外部的成功root登录", alert_msg)
            return  # 成功告警发送后，不再发送尝试告警

    # ======================
    # 登录尝试告警
    # ======================
    if "login:" in payload_l and not session["attempt_alerted"]:
        session["stage"] = "LOGIN_ATTEMPT"
        session["login_time"] = time.time()  # 记录首次尝试时间
        log("STATE", f"{session_key} 进入登录尝试阶段，等待 {LOGIN_GRACE_PERIOD}s 确认是否成功")

# ======================
# 主入口
# ======================
def main():
    print("==============================================")
    print(" Telnet root 登录实时监测与邮件告警系统")
    print(f" 监听接口: {INTERFACE}")
    print("==============================================")

    # sniff 不阻塞，使用后台轮询处理登录尝试超时
    from threading import Thread

    def sniff_thread():
        sniff(iface=INTERFACE, prn=packet_handler, store=False)

    Thread(target=sniff_thread, daemon=True).start()

    while True:
        check_login_timeout()
        time.sleep(1)

if __name__ == "__main__":
    main()
