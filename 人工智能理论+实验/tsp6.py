import matplotlib.pyplot as plt
import numpy as np
import random
import time
from math import sqrt
from typing import List, Tuple, Dict, Callable

# 城市坐标
CITY_COORDINATES = {
    1: (0, 0),
    2: (1, 3),
    3: (4, 2),
    4: (3, 0),
    5: (2, 4)
}

# 算法参数
POPULATION_SIZE = 10 #种群大小
GENERATIONS = 50  # 最大迭代代数
MUTATION_RATE = 0.15 # 变异概率
TOURNAMENT_SIZE = 5 # 锦标赛选择大小

class TSPSolver: # 旅行商问题求解类
    def __init__(self, city_coordinates: dict): # 初始化
        self.city_coordinates = city_coordinates # 城市坐标
        self.city_ids = list(city_coordinates.keys()) # 城市ID
        self.num_cities = len(city_coordinates) # 城市数量
        self.distance_matrix = self._calculate_distance_matrix() # 距离矩阵
        self.results = {}  # 结果存储
        
    def _calculate_distance_matrix(self) -> np.ndarray: # 计算距离矩阵
        matrix = np.zeros((self.num_cities, self.num_cities))
        for i, city1 in enumerate(self.city_ids):
            for j, city2 in enumerate(self.city_ids):
                x1, y1 = self.city_coordinates[city1]
                x2, y2 = self.city_coordinates[city2]
                matrix[i][j] = sqrt((x1 - x2)**2 + (y1 - y2)**2)
        return matrix
    
    def _calculate_path_distance(self, path: List[int]) -> float:  # 计算路径总距离
        total_distance = 0
        for i in range(len(path)):
            city1 = path[i]
            city2 = path[(i + 1) % len(path)]
            idx1 = self.city_ids.index(city1)
            idx2 = self.city_ids.index(city2)
            total_distance += self.distance_matrix[idx1][idx2]
        return total_distance
    
    def _initialize_population(self) -> List[List[int]]: # 初始化种群
        return [random.sample(self.city_ids, len(self.city_ids)) 
                for _ in range(POPULATION_SIZE)]
    
    def _evaluate_fitness(self, individual: List[int]) -> float: # 适应度评估
        distance = self._calculate_path_distance(individual)
        return 1 / (distance + 1e-10)
    
    def _tournament_selection(self, population: List[List[int]]) -> Tuple[List[int], List[int]]: 
        # 锦标赛选择，从种群中随机选择若干个个体，再从中择优选择适应度最好的两个个体
        tournament = random.sample(population, TOURNAMENT_SIZE) 
        parent1 = min(tournament, key=lambda x: self._calculate_path_distance(x))
        tournament = random.sample(population, TOURNAMENT_SIZE) 
        parent2 = min(tournament, key=lambda x: self._calculate_path_distance(x))
        return parent1, parent2
    
    def _roulette_selection(self, population: List[List[int]]) -> Tuple[List[int], List[int]]: 
        # 轮盘赌选择，根据适应度选择个体，适应度越高被选择的概率越大
        fitness_values = [self._evaluate_fitness(ind) for ind in population]
        total_fitness = sum(fitness_values)
        probabilities = [f/total_fitness for f in fitness_values]
        parent1, parent2 = random.choices(population, weights=probabilities, k=2)
        return parent1, parent2
    
    def _pmx_crossover(self, parent1: List[int], parent2: List[int]) -> List[int]: 
        # 部分映射交叉，先选择父代的部分基因，然后根据映射关系填充剩余部分
        size = self.num_cities
        start, end = sorted(random.sample(range(size), 2))
        child = [None] * size
        child[start:end] = parent1[start:end]
        
        mapping = {}
        for i in range(start, end):
            if parent2[i] not in child:
                mapping[parent2[i]] = parent1[i]
        
        for i in list(range(0, start)) + list(range(end, size)):
            gene = parent2[i]
            while gene in mapping:
                gene = mapping[gene]
            child[i] = gene
        
        if len(set(child)) != size:
            return parent1
        return child
    
    def _ox_crossover(self, parent1: List[int], parent2: List[int]) -> List[int]: 
        # 顺序交叉，选择父代的部分基因，然后根据顺序填充剩余部分
        size = self.num_cities
        child = [None] * size
        start, end = sorted(random.sample(range(size), 2))
        child[start:end] = parent1[start:end]
        
        current_pos = end % size
        for city in parent2[end:] + parent2[:end]:
            if city not in child[start:end]:
                child[current_pos] = city
                current_pos = (current_pos + 1) % size
        return child
    
    def _mutate(self, individual: List[int]) -> None: 
        # 随机交换变异
        if random.random() < MUTATION_RATE:
            i, j = random.sample(range(self.num_cities), 2)
            individual[i], individual[j] = individual[j], individual[i]
    
    def run_algorithm(self,  
                    selection_method: Callable,
                    crossover_method: Callable,
                    name: str,
                    use_elitism: bool = True) -> Dict: # 运行算法，可自行配置不同策略
        start_time = time.time()
        population = self._initialize_population()
        best_individual = None
        best_distance = float('inf')
        convergence_data = []
        avg_fitness_data = []
        
        for generation in range(GENERATIONS):
            new_population = []
            
            # 是否选择精英策略，如果选择，则保留当前种群中最好的个体
            if use_elitism:
                population_sorted = sorted(population, key=lambda x: self._calculate_path_distance(x))
                new_population.extend(population_sorted[:3])  # 保留3个精英
            
            # 按所选策略进行选择和交叉
            while len(new_population) < POPULATION_SIZE:
                parent1, parent2 = selection_method(population)
                child = crossover_method(parent1, parent2)
                self._mutate(child)
                new_population.append(child)
            
            population = new_population
            
            # 计算当前种群的最佳个体和平均适应度
            current_best = min(population, key=lambda x: self._calculate_path_distance(x))
            current_distance = self._calculate_path_distance(current_best)
            avg_fitness = np.mean([self._evaluate_fitness(ind) for ind in population])
            
            if current_distance < best_distance:
                best_individual = current_best
                best_distance = current_distance
            
            convergence_data.append(best_distance)
            avg_fitness_data.append(avg_fitness)
            
            # 每代输出
            print(f"{name} - Gen {generation+1}: Best={current_distance:.2f}, AvgFit={avg_fitness:.4f}, Path={current_best}")
        
        runtime = time.time() - start_time # 计算运行时间
        return {
            'name': name,
            'best_path': best_individual,
            'best_distance': best_distance,
            'runtime': runtime,
            'convergence': convergence_data,
            'avg_fitness': avg_fitness_data,
            'use_elitism': use_elitism
        }
    
    def visualize_results(self): # 可视化结果
        plt.figure(figsize=(15, 10))
        
        # 收敛曲线
        plt.subplot(2, 2, 1)
        for result in self.results.values():
            linestyle = '-' if result['use_elitism'] else '--'
            plt.plot(result['convergence'], linestyle=linestyle, 
                    label=f"{result['name']} (Final: {result['best_distance']:.2f})")
        plt.title("Convergence Analysis")
        plt.xlabel("Generation")
        plt.ylabel("best_distance")
        plt.legend()
        plt.grid(True)
        
        # 适应度曲线
        plt.subplot(2, 2, 2)
        for result in self.results.values():
            linestyle = '-' if result['use_elitism'] else '--'
            plt.plot(result['avg_fitness'], linestyle=linestyle, 
                    label=f"{result['name']}")
        plt.title("Average Fitness Analysis")
        plt.xlabel("Generation")
        plt.ylabel("Average Fitness")
        plt.legend()
        plt.grid(True)
        
        # 运行时间比较
        plt.subplot(2, 2, 3)
        names = [r['name'] for r in self.results.values()]
        runtimes = [r['runtime'] for r in self.results.values()]
        colors = ['blue' if r['use_elitism'] else 'orange' for r in self.results.values()]
        plt.bar(names, runtimes, color=colors)
        plt.title("runningTime_compare/second")
        plt.xticks(rotation=45)
        plt.grid(True)
        
        # 最终结果比较
        plt.subplot(2, 2, 4)
        final_distances = [r['best_distance'] for r in self.results.values()]
        plt.bar(names, final_distances, color=colors)
        plt.title("Final Distance Comparison")
        plt.xticks(rotation=45)
        plt.grid(True)
        
        plt.tight_layout()
        plt.show()
        
        # 显示所有最佳路径
        for name, result in self.results.items():
            self.visualize_path(result['best_path'], 
                              f"best_path - {name} (distance: {result['best_distance']:.2f})")
    
    def visualize_path(self, path: List[int], title: str):
        x = [self.city_coordinates[city][0] for city in path]
        y = [self.city_coordinates[city][1] for city in path]
        x.append(x[0])
        y.append(y[0])
        
        plt.figure(figsize=(6, 5))
        plt.plot(x, y, 'o-', markersize=8)
        
        for i, city in enumerate(path):
            plt.text(self.city_coordinates[city][0], 
                    self.city_coordinates[city][1], 
                    str(city), fontsize=12, ha='center', va='bottom')
        
        plt.title(title)
        plt.xlabel("X ")
        plt.ylabel("Y ")
        plt.grid(True)
        plt.show()

if __name__ == "__main__":
    solver = TSPSolver(CITY_COORDINATES)
    
    # 运行6种组合（4种带精英策略，2种不带）
    combinations = [
        ('Tournament+PMX', solver._tournament_selection, solver._pmx_crossover, True),
        ('Tournament+OX', solver._tournament_selection, solver._ox_crossover, True),
        ('Roulette+PMX', solver._roulette_selection, solver._pmx_crossover, True),
        ('Roulette+OX', solver._roulette_selection, solver._ox_crossover, True),
        ('Roulette+PMX (No Elitism)', solver._roulette_selection, solver._pmx_crossover, False),
        ('Roulette+OX (No Elitism)', solver._roulette_selection, solver._ox_crossover, False)
    ]
    
    for name, selection, crossover, elitism in combinations:
        print(f"\n运行时间 {name}...")
        result = solver.run_algorithm(selection, crossover, name, elitism)
        solver.results[name] = result
        print(f"{name} completed in {result['runtime']:.2f} seconds")
        print(f"最短距离: {result['best_distance']:.2f}")
        print(f"最佳路径: {result['best_path']}")
    
    # 比较分析
    print("\n比较分析:")
    for name, result in solver.results.items():
        print(f"{name}: Time={result['runtime']:.2f}s, Best={result['best_distance']:.2f}")
    
    solver.visualize_results()
