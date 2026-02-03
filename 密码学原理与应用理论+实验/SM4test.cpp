#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>

using namespace std;

const unsigned int FK[4] = {0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc};//
unsigned int CK[32];

unsigned int Sbox[256] = {
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
    0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
    0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
    0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
    0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
    0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
    0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
    0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
    0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
    0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
};


unsigned int RotL(unsigned int In, int loop)//
{
    return	(In << loop) | (In >> (32 - loop)); }

unsigned int L_Func(unsigned int In)//循环左移再异或
{
	return  In ^ RotL(In,2) ^ RotL(In,10) ^ RotL(In,18) ^ RotL(In,24);}

unsigned int L1_Func(unsigned int In)
{
	return  In ^ RotL(In,13) ^ RotL(In,23);}

unsigned int S_Func(unsigned int In)
{
	unsigned int Out = 0;
	unsigned char temp = {0};
	for(int i = 0; i<4; i++)              //4组8bit的部分分别经过S盒, 再拼成32bit
	{
		temp = ((In >> (24 - 8 * i)) & 0xFF);
		Out = Out + (Sbox[temp] << (24 - 8 * i));
	}
	return Out; 
}

unsigned int T(unsigned int In)
{return L_Func(S_Func(In)); 	}

unsigned int T1(unsigned int In)
{
	return L1_Func(S_Func(In)); 	
}

void SetPara()                               //设置所有固定参数CKi
{
	unsigned int temp = 0;
	for(int i = 0; i<32; i++)
	{
		for(int j = 0; j <4; j++)
		{
			temp = (7 * (4 * i + j)) & 0xFF;   
			temp = temp << (24 - 8 * j);
			CK[i] = CK[i] + temp;
		}
	}
}


void SetRoundKey(unsigned int SK[], const unsigned int MK[]) {
    //密钥扩展
    unsigned int K[36] = {0};
    for (int i = 0; i < 4; i++) {
        K[i] = MK[i] ^ FK[i];
    }
    for (int i = 0; i < 32; i++) {
        K[i + 4] = K[i] ^ T1(K[i + 1] ^ K[i + 2] ^ K[i + 3] ^ CK[i]);
        SK[i] = K[i + 4];
    }
}

void SM4Crypt(unsigned int Out[], const unsigned int In[], const unsigned int SK[], bool flag) {
    unsigned int X[36] = {0};
    for (int i = 0; i < 4; i++) {
        X[i] = In[i];
    }
    for (int i = 0; i < 32; i++) {
        int j = flag ? i : 31 - i;    //flag为1，加密，正取轮密钥
        X[i + 4] = X[i] ^ T(X[i + 1] ^ X[i + 2] ^ X[i + 3] ^ SK[j]);
    }
    for (int i = 0; i < 4; i++) {
        Out[i] = X[35 - i];
    }
}

// 普通字符串-字节数组
void string_to_bytes(const std::string& str, std::vector<unsigned char>& bytes) {
    for (size_t i = 0; i < str.size(); ++i) {
        bytes.push_back(static_cast<unsigned char>(str[i]));
    }
}

string bytes_to_string(const std::vector<unsigned char>& bytes) {
    std::string str(bytes.begin(), bytes.end());
    return str;
}
// PKCS#7 填充
void pkcs7_pad(std::vector<unsigned char>& bytes, size_t block_size) {
    size_t pad_len = block_size - (bytes.size() % block_size);
    bytes.insert(bytes.end(), pad_len, static_cast<unsigned char>(pad_len));
}

// 去除 PKCS#7 填充
void pkcs7_unpad(std::vector<unsigned char>& bytes) {
    if (bytes.empty()) return;
    unsigned char pad_len = bytes.back();//返回最后一个元素，即填充长度
    if (pad_len > 0 && pad_len <= bytes.size()) {
        bytes.resize(bytes.size() - pad_len);
    }
}

// 将字节数组转换为 unsigned int 数组
void bytes_to_uint_array(const std::vector<unsigned char>& bytes, unsigned int arr[]) {
    for (size_t i = 0; i < 4; ++i) {
        arr[i] = 0;
        for (size_t j = 0; j < 4; ++j) {
            arr[i] |= bytes[i * 4 + j] << (24 - j * 8);
        }
    }
}

// 将 unsigned int 数组转换为字节数组
void uint_array_to_bytes(const unsigned int arr[], std::vector<unsigned char>& bytes) {
    bytes.clear();
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            bytes.push_back((arr[i] >> (24 - j * 8)) & 0xFF);
        }
    }
}

// 将字节数组转换为十六进制字符串
string bytes_to_hex_string(const std::vector<unsigned char>& bytes) {
    std::stringstream ss;
    for (size_t i = 0; i < bytes.size(); ++i) {
        ss << hex << setw(2) << setfill('0') << (int)bytes[i];
    }
    return ss.str();
}

int main() {
    string plaintext, key;
    unsigned int In[4] = {0};
    unsigned int Out[4] = {0};
    unsigned int SK[32] = {0};

    
    cout << "请输入明文:" << endl;
    getline(cin, plaintext);
    cout << "请输入密钥:" << endl;
    getline(cin, key);

    vector<unsigned char> plaintext_bytes;
    vector<unsigned char> key_bytes;

    string_to_bytes(plaintext, plaintext_bytes);
    string_to_bytes(key, key_bytes);
    //填充
    pkcs7_pad(plaintext_bytes, 16);
    pkcs7_pad(key_bytes, 16);

    SetPara();
    bytes_to_uint_array(key_bytes, SK);
    SetRoundKey(SK, SK);

    vector<unsigned char> encrypted_bytes;
    for (size_t i = 0; i < plaintext_bytes.size(); i += 16) {//分块加密 
        vector<unsigned char> block(plaintext_bytes.begin() + i, plaintext_bytes.begin() + i + 16);
        bytes_to_uint_array(block, In);
        SM4Crypt(Out, In, SK, true);
        vector<unsigned char> encrypted_block;
        uint_array_to_bytes(Out, encrypted_block);
        encrypted_bytes.insert(encrypted_bytes.end(), encrypted_block.begin(), encrypted_block.end());
    }

    string encrypted_hex_string = bytes_to_hex_string(encrypted_bytes);
    cout << "密文: " << encrypted_hex_string << endl;
    string encrypted_string = bytes_to_string(encrypted_bytes);
    //cout<<encrypted_string<<endl;

    vector<unsigned char> decrypted_bytes;
    for (size_t i = 0; i < encrypted_bytes.size(); i += 16) {
        vector<unsigned char> block(encrypted_bytes.begin() + i, encrypted_bytes.begin() + i + 16);
        bytes_to_uint_array(block, In);
        SM4Crypt(Out, In, SK, false);
        vector<unsigned char> decrypted_block;
        uint_array_to_bytes(Out, decrypted_block);
        decrypted_bytes.insert(decrypted_bytes.end(), decrypted_block.begin(), decrypted_block.end());
    }

    pkcs7_unpad(decrypted_bytes);
    cout << "解密后的明文: ";
    for (size_t i = 0; i < decrypted_bytes.size(); i++) {
        cout << decrypted_bytes[i];
    }
    cout << endl;

    return 0;
}