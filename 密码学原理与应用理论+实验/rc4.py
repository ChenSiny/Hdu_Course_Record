def sboxcreate(key):
    sbox = list(range(256))
    j = 0
    for i in range(256):
        j = (j + sbox[i] + key[i % len(key)]) % 256
        sbox[i], sbox[j] = sbox[j], sbox[i]
    return sbox

def keycreate(sbox, length):
    i = j = 0
    key_stream = []
    for _ in range(length):
        i = (i + 1) % 256
        j = (j + sbox[i]) % 256
        sbox[i], sbox[j] = sbox[j], sbox[i]
        t = (sbox[i] + sbox[j]) % 256
        key_stream.append(sbox[t])
    return key_stream

def rc4_encrypt_decrypt(data, key):
    key = [ord(c) for c in key]
    sbox = sboxcreate(key)
    key_stream = keycreate(sbox, len(data))
    return [d ^ k for d, k in zip(data, key_stream)]

def string_to_binary(s):
    return ' '.join(format(ord(char), '08b') for char in s)

def binary_to_string(b):
    binary_values = b.split()
    ascii_characters = [chr(int(binary, 2)) for binary in binary_values]
    return ''.join(ascii_characters)

if __name__ == '__main__':
    print("请输入明文：")
    plain_text = input()
    print("明文的二进制格式：")
    bin_plain_text = string_to_binary(plain_text)
    print(bin_plain_text)

    print("请输入5-16字符密钥：")
    key = input()

    print("密钥流的二进制格式：")
    key_stream = keycreate(sboxcreate([ord(c) for c in key]), len(plain_text))
    bin_key_stream = ' '.join(format(k, '08b') for k in key_stream)
    print(bin_key_stream)

    encrypted_data = rc4_encrypt_decrypt([ord(c) for c in plain_text], key)
    encrypted_bin = ' '.join(format(e, '08b') for e in encrypted_data)
    encrypted_text = ''.join(chr(e) for e in encrypted_data)
    print("加密后密文的二进制格式：")
    print(encrypted_bin)
    print("密文：")
    print(encrypted_text)

    decrypted_data = rc4_encrypt_decrypt(encrypted_data, key)
    decrypted_bin = ' '.join(format(d, '08b') for d in decrypted_data)
    decrypted_text = ''.join(chr(d) for d in decrypted_data)
    print("解密后明文的二进制格式：")
    print(decrypted_bin)
    print("明文：")
    print(decrypted_text)
