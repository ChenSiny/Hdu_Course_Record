import networkx as nx
import matplotlib.pyplot as plt
from matplotlib import font_manager
import matplotlib
# 设置中文字体（防止中文乱码）
my_font = font_manager.FontProperties(fname="/System/Library/Fonts/Supplemental/Songti.ttc")
matplotlib.rcParams['font.family'] = my_font.get_name()
# 规则库
rules = [
    {"conditions": ["咳嗽", "发烧"], "conclusion": "疑似呼吸道感染"},
    {"conditions": ["喉咙痛"], "conclusion": "疑似咽喉刺激"},
    {"conditions": ["疑似咽喉刺激", "疑似呼吸道感染"], "conclusion": "疑似上呼吸道感染"},
    {"conditions": ["疑似上呼吸道感染"], "conclusion": "建议就诊科室为耳鼻喉科"},
    {"conditions": ["胸闷", "气短"], "conclusion": "疑似肺部问题"},
    {"conditions": ["疑似肺部问题", "疑似呼吸道感染"], "conclusion": "疑似肺炎"},
    {"conditions": ["疑似肺炎"], "conclusion": "建议就诊科室为呼吸科"},
    {"conditions": ["腹痛"], "conclusion": "疑似胃肠疾病"},
    {"conditions": ["疑似胃肠疾病", "腹泻"], "conclusion": "疑似肠炎"},
    {"conditions": ["疑似肠炎"], "conclusion": "建议就诊科室为消化内科"},
    {"conditions": ["心悸", "头晕"], "conclusion": "疑似心律不齐"},
    {"conditions": ["疑似心律不齐"], "conclusion": "建议就诊科室为心内科"},
    {"conditions": ["胸痛"], "conclusion": "建议就诊科室为心内科"},
    {"conditions": ["皮疹", "瘙痒"], "conclusion": "疑似皮肤过敏"},
    {"conditions": ["疑似皮肤过敏"], "conclusion": "建议就诊科室为皮肤科"}
]

# 推理函数
def inference(symptoms):
    known = set(symptoms) 
    trace = {}
    for symptom in symptoms:
        trace[symptom] = {'layer': 0, 'label': symptom, 'children': [], 'type': '症状'}
    while True:
        applied = False
        for rule in rules:
            if all(cond in known for cond in rule['conditions']) and rule['conclusion'] not in known:
                known.add(rule['conclusion'])
                applied = True
                if rule['conclusion'].startswith("建议就诊科室为"):
                    node_type = "建议"
                elif rule['conclusion'].startswith("疑似"):
                    node_type = "疾病"
                else:
                    node_type = "中间推理"
                trace[rule['conclusion']] = {
                    'layer': None,
                    'label': rule['conclusion'],
                    'children': [],
                    'type': node_type
                }
                for cond in rule['conditions']:
                    trace[cond]['children'].append(rule['conclusion'])
        if not applied:
            break

    # 给节点分层
    def assign_layer(node, current_layer):
        if trace[node]['layer'] is None or trace[node]['layer'] < current_layer:
            trace[node]['layer'] = current_layer
        for child in trace[node]['children']:
            assign_layer(child, current_layer + 1)

    for symptom in symptoms:
        assign_layer(symptom, 0)

    return known - set(symptoms), trace


def visualize_inference(trace, title):
    G = nx.DiGraph()
    for node, info in trace.items():
        G.add_node(node, layer=info['layer'], label=info['label'], type=info['type'])
    for node, info in trace.items():
        for child in info.get('children', []):
            G.add_edge(node, child)
    # 布局处理
    layers = {}
    for node, data in G.nodes(data=True):
        layer = data['layer']
        if layer not in layers:
            layers[layer] = []
        layers[layer].append(node)
    pos = {}
    y_gap = 2
    for layer_idx, (layer, nodes_in_layer) in enumerate(sorted(layers.items())):
        x_gap = 2
        x_start = -(len(nodes_in_layer) - 1) * x_gap / 2
        for i, node in enumerate(nodes_in_layer):
            pos[node] = (x_start + i * x_gap, -layer_idx * y_gap)
    color_map = {
        "症状": "lightgreen",
        "疾病": "lightblue",
        "建议": "orange",
        "中间推理": "lightgrey"
    }
    shape_map = {
        "症状": "o",    # circle
        "疾病": "s",    # square
        "建议": "D",    # diamond
        "中间推理": "v" # triangle down
    }
    plt.figure(figsize=(10, 6))
    # 按类型绘制不同形状
    for node_type in color_map:
        nodelist = [n for n, d in G.nodes(data=True) if d['type'] == node_type]
        nx.draw_networkx_nodes(
            G, pos, nodelist=nodelist, 
            node_color=color_map[node_type], 
            node_shape=shape_map[node_type],
            node_size=2000,
            label=node_type
        )

    nx.draw_networkx_edges(G, pos, arrows=True)
    labels = nx.get_node_attributes(G, 'label')
    nx.draw_networkx_labels(G, pos, labels=labels, font_size=10, font_family=my_font.get_name())

    plt.title(title, fontproperties=my_font, fontsize=15)
    plt.legend(scatterpoints=1, markerscale=0.3, fontsize=10, loc="lower left")
    plt.axis('off')
    plt.show()

# ------------------------
# 测试：病人A
patient_A_symptoms = ["咳嗽", "发烧", "喉咙痛", "胸闷"]
results_A, trace_A = inference(patient_A_symptoms)

print("\n病人A的推理结果：")
for res in results_A:
    print(" -", res)

visualize_inference(trace_A, "病人A推理图")

# 测试：病人B
patient_B_symptoms = ["腹痛", "腹泻"]
results_B, trace_B = inference(patient_B_symptoms)

print("\n病人B的推理结果：")
for res in results_B:
    print(" -", res)

visualize_inference(trace_B, "病人B推理图")
