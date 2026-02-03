import heapq
from time import time

class Node:
    def __init__(self, state, parent=None, move=None, depth=0):
        self.state = state  # 当前状态
        self.parent = parent  # 父节点
        self.move = move  # 当前移动方向
        self.depth = depth  # 深度
        self.g = 0  # 从起点到当前节点的代价
        self.h = 0  # 启发式估计值
        self.f = 0  # 总代价

    def __lt__(self, other):
        return self.f < other.f

def a_star_eight_puzzle(start, goal):

    open_list = []
    closed_list = set()
    start_node = Node(start)
    goal_node = Node(goal)

    start_node.h = heuristic(start, goal)
    start_node.f = start_node.h
    heapq.heappush(open_list, start_node)

    while open_list:
        # 打印 open表 和 close表
        print("Open List:", [node.state for node in open_list])
        print("Closed List:", list(closed_list))
        print()

        current_node = heapq.heappop(open_list) #将当前状态移出open表并维持顶堆

        # 如果找到目标状态
        if current_node.state == goal_node.state:
            return reconstruct_path(current_node)

        closed_list.add(tuple(current_node.state)) #将当前状态加入close表

        # 获取当前状态的所有可能移动
        for move, new_state in get_neighbors(current_node.state):
            if tuple(new_state) in closed_list: #排除已遍历状态
                continue

            neighbor_node = Node(new_state, current_node, move, current_node.depth + 1)
            neighbor_node.g = current_node.g + 1
            neighbor_node.h = heuristic(new_state, goal)
            neighbor_node.f = neighbor_node.g + neighbor_node.h

            # 如果邻居节点已经在open_list中，检查是否有更优路径
            if any(node.state == neighbor_node.state and node.f <= neighbor_node.f for node in open_list):
                continue

            heapq.heappush(open_list, neighbor_node)

    return None  # 无法到达目标状态

def get_neighbors(state):
    neighbors = []
    zero_index = state.index(0)
    row, col = divmod(zero_index, 3)
    moves = {
        'UP': (row - 1, col),
        'DOWN': (row + 1, col),
        'LEFT': (row, col - 1),
        'RIGHT': (row, col + 1)
    }

    for move, (new_row, new_col) in moves.items():
        if 0 <= new_row < 3 and 0 <= new_col < 3:
            new_index = new_row * 3 + new_col
            new_state = state[:]
            new_state[zero_index], new_state[new_index] = new_state[new_index], new_state[zero_index]
            neighbors.append((move, new_state))

    return neighbors

def heuristic(state, goal):
    distance = 0
    for i, value in enumerate(state):
        if value == 0:
            continue
        goal_index = goal.index(value)
        current_row, current_col = divmod(i, 3)
        goal_row, goal_col = divmod(goal_index, 3)
        distance += abs(current_row - goal_row) + abs(current_col - goal_col)
    return distance

def reconstruct_path(node):
    path = []
    while node.parent:
        path.append(node.move)
        node = node.parent
    return path[::-1]

def print_state(state):
    for i in range(0, 9, 3):
        print(state[i:i+3])
    print()

def visualize_solution(start, path):
    state = start[:]
    print("初始状态:")
    print_state(state)
    for move in path:
        zero_index = state.index(0)
        row, col = divmod(zero_index, 3)
        if move == 'UP':
            new_row, new_col = row - 1, col
        elif move == 'DOWN':
            new_row, new_col = row + 1, col
        elif move == 'LEFT':
            new_row, new_col = row, col - 1
        elif move == 'RIGHT':
            new_row, new_col = row, col + 1
        new_index = new_row * 3 + new_col
        state[zero_index], state[new_index] = state[new_index], state[zero_index]
        print(f"移动: {move}")
        print_state(state)


start_state = [2,8,3,1,0,4,7,6,5]  # 初始状态
goal_state = [1,2,3,8,0,4,7,6,5]  # 目标状态
start_time=time()
solution = a_star_eight_puzzle(start_state, goal_state)
end_time=time()
if solution:
    print(f"搜索耗时: {end_time - start_time:.4f} 秒") 
    print("解决方案:", solution)
    visualize_solution(start_state, solution)
else:
    print("无法找到解决方案")