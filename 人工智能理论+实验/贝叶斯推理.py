import matplotlib.pyplot as plt
import networkx as nx
from pgmpy.models import DiscreteBayesianNetwork
from pgmpy.factors.discrete import TabularCPD
from pgmpy.inference import VariableElimination


model = DiscreteBayesianNetwork([
    ('Smoking', 'Bronchitis'), # 吸烟 -> 气管炎
    ('Cold', 'Bronchitis'),  # 感冒 -> 气管炎
    ('Bronchitis', 'Cough'),  # 气管炎 -> 咳嗽
    ('Bronchitis', 'Wheeze')  # 气管炎 -> 气喘
])

cpd_smoking = TabularCPD(variable='Smoking', variable_card=2, values=[[0.6], [0.4]])
cpd_cold = TabularCPD(variable='Cold', variable_card=2, values=[[0.8], [0.2]])

cpd_bronchitis = TabularCPD(
    variable='Bronchitis', variable_card=2,
    values=[
        [0.35, 0.25, 0.011, 0.002],  # P(B=是)
        [0.65, 0.75, 0.989, 0.998]   # P(B=否)
    ],
    evidence=['Smoking', 'Cold'],
    evidence_card=[2, 2]
)

cpd_cough = TabularCPD(
    variable='Cough', variable_card=2,
    values=[
        [0.85, 0.15],  # P(Co=是)
        [0.15, 0.85]   # P(Co=否)
    ],
    evidence=['Bronchitis'],
    evidence_card=[2]
)

cpd_wheeze = TabularCPD(
    variable='Wheeze', variable_card=2,
    values=[
        [0.5, 0.1],  # P(W=是)
        [0.5, 0.9]   # P(W=否)
    ],
    evidence=['Bronchitis'],
    evidence_card=[2]
)


model.add_cpds(cpd_smoking, cpd_cold, cpd_bronchitis, cpd_cough, cpd_wheeze)
assert model.check_model()

infer = VariableElimination(model)

print("\nP(气管炎 | 吸烟=是, 感冒=是):")
result1 = infer.query(['Bronchitis'], evidence={'Smoking': 0, 'Cold': 0})
print(result1)

print("\nP(咳嗽 | 气管炎=是):")
result2 = infer.query(['Cough'], evidence={'Bronchitis': 0})
print(result2)

print("\nP(咳嗽 | 吸烟=是):")
result3 = infer.query(['Cough'], evidence={'Smoking': 0})
print(result3)



pos = {
    'Smoking': (-2, 2),     # 左上角
    'Cold': (2, 2),         # 右上角
    'Bronchitis': (0, 1),   # 中间层
    'Cough': (-1, 0),       # 左下角
    'Wheeze': (1, 0)        # 右下角
}

# 绘图
plt.figure(figsize=(8, 6))
nx.draw(model, pos=pos, with_labels=True,
        node_size=3000, node_color='lightblue',
        font_size=12, arrows=True, arrowsize=20)

plt.title("贝叶斯网络结构图", fontsize=14)
plt.axis('off')
plt.show()