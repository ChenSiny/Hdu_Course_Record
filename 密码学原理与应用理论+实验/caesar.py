import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton, QLabel

# 凯撒加密函数
def caesar_encrypt(text, shift):
    result = ""
    for c in text:
        if c.isalpha():
            base = 'a' if c.islower() else 'A'
            c = chr((ord(c) - ord(base) + shift) % 26 + ord(base))
        result += c
    return result

# 凯撒解密函数
def caesar_decrypt(text, shift):
    return caesar_encrypt(text, 26 - shift)

class CaesarCipherWidget(QWidget):
    def __init__(self):
        super().__init__()

        main_layout = QVBoxLayout()

        input_layout = QHBoxLayout()
        input_label = QLabel("Input:")
        self.input_text = QLineEdit()
        input_layout.addWidget(input_label)
        input_layout.addWidget(self.input_text)

        output_layout = QHBoxLayout()
        output_label = QLabel("Output:")
        self.output_text = QLineEdit()
        self.output_text.setReadOnly(True)
        output_layout.addWidget(output_label)
        output_layout.addWidget(self.output_text)

        button_layout = QHBoxLayout()
        encrypt_button = QPushButton("Encrypt")
        decrypt_button = QPushButton("Decrypt")
        button_layout.addWidget(encrypt_button)
        button_layout.addWidget(decrypt_button)

        main_layout.addLayout(input_layout)
        main_layout.addLayout(output_layout)
        main_layout.addLayout(button_layout)

        self.setLayout(main_layout)

        encrypt_button.clicked.connect(self.encrypt_text)
        decrypt_button.clicked.connect(self.decrypt_text)

    def encrypt_text(self):
        text = self.input_text.text()
        shift = 3  # 可以根据需要修改
        encrypted = caesar_encrypt(text, shift)
        self.output_text.setText(encrypted)

    def decrypt_text(self):
        text = self.input_text.text()
        shift = 3  # 可以根据需要修改
        decrypted = caesar_decrypt(text, shift)
        self.output_text.setText(decrypted)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    widget = CaesarCipherWidget()
    widget.setWindowTitle("Caesar Cipher")
    widget.show()

    sys.exit(app.exec_())