import numpy as np
import matplotlib.pyplot as plt
from matplotlib import font_manager

# 设置浮点数输出格式：保留一位小数，如 1.0 而非 1.
np.set_printoptions(formatter={'float_kind': lambda x: "%.1f" % x})

my_font = font_manager.FontProperties(fname="/System/Library/Fonts/Supplemental/Songti.ttc")

A = np.array([1, 1, 0.5, 0.2, 0])
print("输入模糊集合 A':", A)
R = np.array([
    [0.5, 0.5, 0.5, 0.8, 1.0],
    [0.5, 0.5, 0.5, 0.8, 1.0],
    [0.5, 0.5, 0.5, 0.5, 0.5],
    [0.2, 0.2, 0.2, 0.2, 0.2],
    [0.0, 0.0, 0.0, 0.0, 0.0]
])
print("\n模糊关系矩阵 R:")
print(R)
# Max-Min 推理
def max_min_fuzzy_inference(A, R):
    B = np.zeros(R.shape[1])
    print("\n--- 合成运算过程（Max-Min） ---")
    for j in range(R.shape[1]):
        min_values = np.minimum(A, R[:, j])
        B[j] = np.max(min_values)
        print(f"\n隶属度 {j+1}:")
        for i in range(len(A)):
            print(f"min(A[{i}]={A[i]}, R[{i}][{j}]={R[i][j]}) = {min_values[i]}")
        print(f"max(...) = {B[j]}")
    return B

# Max-Prod 推理
def max_prod_fuzzy_inference(A, R):
    B = np.zeros(R.shape[1])
    print("\n--- 合成运算过程（Max-Prod） ---")
    for j in range(R.shape[1]):
        prod_values = A * R[:, j]
        B[j] = np.max(prod_values)
        print(f"\n隶属度 {j+1}:")
        for i in range(len(A)):
            print(f"prod(A[{i}]={A[i]}, R[{i}][{j}]={R[i][j]}) = {prod_values[i]:.1f}")
        print(f"max(...) = {B[j]:.1f}")
    return B

# 去模糊化（重心法）
def defuzzify_centroid(B):
    x = np.arange(1, len(B)+1)
    numerator = np.sum(x * B)
    denominator = np.sum(B)
    result = numerator / denominator if denominator != 0 else 0
    print("\n--- 去模糊化（重心法） ---")
    print("输出模糊集合 B:", B)
    print("numerator =", numerator)
    print("denominator =", denominator)
    print("去模糊化结果 =", result)
    return result

# 可视化函数：使用条形图
def plot_membership(title, x, y):
    plt.bar(x, y, width=0.5, align='center')
    plt.title(title, fontproperties=my_font)
    plt.xlabel("等级", fontproperties=my_font)
    plt.ylabel("隶属度", fontproperties=my_font)
    plt.xticks(x)
    plt.ylim(0, 1.05)
    plt.grid(axis='y')

x = np.arange(1, 6)

# Max-Min 推理
B_max_min = max_min_fuzzy_inference(A, R)
crisp_max_min = defuzzify_centroid(B_max_min)

# Max-Prod 推理
B_max_prod = max_prod_fuzzy_inference(A, R)
crisp_max_prod = defuzzify_centroid(B_max_prod)

# 可视化输入输出隶属度集合
plt.figure(figsize=(12, 6))

plt.subplot(2, 2, 1)
plot_membership("输入模糊集合 A'", x, A)

plt.subplot(2, 2, 2)
plot_membership("输出模糊集合 B (Max-Min)", x, B_max_min)

plt.subplot(2, 2, 4)
plot_membership("输出模糊集合 B (Max-Prod)", x, B_max_prod)

plt.tight_layout()
plt.show()

