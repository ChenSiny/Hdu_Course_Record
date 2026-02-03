from pgmpy.models import DiscreteBayesianNetwork
from pgmpy.factors.discrete import TabularCPD
from pgmpy.inference import VariableElimination

# 构建贝叶斯网络结构
model = DiscreteBayesianNetwork([
    ('Cough', 'SuspectedRespInfection'),
    ('Fever', 'SuspectedRespInfection'),
    ('SoreThroat', 'ThroatIrritation'),
    ('ThroatIrritation', 'UpperRespInfection'),
    ('SuspectedRespInfection', 'UpperRespInfection'),
    ('ChestTightness', 'SuspectedLungIssue'),
    ('ShortnessOfBreath', 'SuspectedLungIssue'),
    ('SuspectedLungIssue', 'Pneumonia'),
    ('SuspectedRespInfection', 'Pneumonia'),
])

# 定义 CPDs（条件概率表）
cpd_cough = TabularCPD('Cough', 2, [[0.5], [0.5]])
cpd_fever = TabularCPD('Fever', 2, [[0.5], [0.5]])
cpd_sore_throat = TabularCPD('SoreThroat', 2, [[0.5], [0.5]])
cpd_chest_tightness = TabularCPD('ChestTightness', 2, [[0.5], [0.5]])
cpd_shortness_breath = TabularCPD('ShortnessOfBreath', 2, [[0.5], [0.5]])

# P(呼吸道感染 | 咳嗽, 发烧)
cpd_sus_resp = TabularCPD('SuspectedRespInfection', 2,
    [[0.1, 0.5, 0.4, 0.9],   # False
     [0.9, 0.5, 0.6, 0.1]],  # True
    evidence=['Cough', 'Fever'],
    evidence_card=[2, 2]
)

# P(咽喉刺激 | 喉咙痛)
cpd_throat_irr = TabularCPD('ThroatIrritation', 2,
    [[0.05, 0.95],  # False
     [0.95, 0.05]], # True
    evidence=['SoreThroat'],
    evidence_card=[2]
)

# P(上呼吸道感染 | 呼吸道感染, 咽喉刺激)
cpd_upper_resp = TabularCPD('UpperRespInfection', 2,
    [[0.01, 0.6, 0.7, 0.05],  # False
     [0.99, 0.4, 0.3, 0.95]], # True
    evidence=['SuspectedRespInfection', 'ThroatIrritation'],
    evidence_card=[2, 2]
)

# P(肺部问题 | 胸闷, 气短)
cpd_lung_issue = TabularCPD('SuspectedLungIssue', 2,
    [[0.1, 0.5, 0.4, 0.1],    # False
     [0.9, 0.5, 0.6, 0.9]],   # True
    evidence=['ChestTightness', 'ShortnessOfBreath'],
    evidence_card=[2, 2]
)

# P(肺炎 | 呼吸道感染, 肺部问题)
cpd_pneumonia = TabularCPD('Pneumonia', 2,
    [[0.01, 0.5, 0.6, 0.05],  # False
     [0.99, 0.5, 0.4, 0.95]], # True
    evidence=['SuspectedRespInfection', 'SuspectedLungIssue'],
    evidence_card=[2, 2]
)

# 添加所有CPD到模型
model.add_cpds(
    cpd_cough, cpd_fever, cpd_sore_throat,
    cpd_chest_tightness, cpd_shortness_breath,
    cpd_sus_resp, cpd_throat_irr,
    cpd_upper_resp, cpd_lung_issue,
    cpd_pneumonia
)
# 检查模型合法性
assert model.check_model()
# 进行推理
inference = VariableElimination(model)
evidence = {
    'Cough': 1,
    'Fever': 1,
    'SoreThroat': 1,
    'ChestTightness': 0,
    'ShortnessOfBreath': 0
}
print("---诊断推理：咳嗽、发烧、胸闷病人诊断结果---")
q2 = inference.query(variables=['Pneumonia'], evidence=evidence)
print("\n肺炎概率：")
print(q2)
print("\n---因果推理：已知患肺炎，推断咳嗽可能性---")
q3 = inference.query(variables=['Cough'], evidence={'Pneumonia': 1})
print("Cough | Pneumonia=1:")
print(q3)