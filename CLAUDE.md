Always use Context7 MCP when I need library/API documentation, code generation, setup or configuration steps without me having to explicitly ask.

Manim可视化工程模板

使用约定：
1. 每个可视化单独子文件夹：名称_算法_日期（如：二叉树_BFS_20260210）
2. 文件结构：
   项目根目录/
   └── manimStudio/
       ├── scenes/
       │   └── algorithm_name.py
       └── media/ (自动生成)

3. LLM生成：只写 manimStudio/scenes/algorithm_name.py，文件名和类名都用英文
4. 用户渲染：uv run manim manimStudio/scenes/algorithm_name.py ClassName -pql --media_dir manimStudio/media

示例场景类结构：
class AlgorithmName(Scene):
    def construct(self):
        #1️⃣ 准备数据结构
        #2️⃣ 显示初始状态  
        #3️⃣ 逐步动画演示
        #4️⃣ 显示结果
        #5️⃣ 结束等待

渲染命令：uv run manim manimStudio/scenes/algorithm_name.py ClassName -pql --media_dir manimStudio/media