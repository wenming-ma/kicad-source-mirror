const { Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell,
        HeadingLevel, BorderStyle, WidthType, ShadingType, VerticalAlign,
        AlignmentType, LevelFormat } = require('docx');
const fs = require('fs');

// 表格边框配置
const tableBorder = { style: BorderStyle.SINGLE, size: 1, color: "CCCCCC" };
const cellBorders = {
  top: tableBorder,
  bottom: tableBorder,
  left: tableBorder,
  right: tableBorder
};

// 创建表格单元格辅助函数
function createHeaderCell(text, width = 1500) {
  return new TableCell({
    borders: cellBorders,
    width: { size: width, type: WidthType.DXA },
    shading: { fill: "D5E8F0", type: ShadingType.CLEAR },
    verticalAlign: VerticalAlign.CENTER,
    children: [new Paragraph({
      alignment: AlignmentType.CENTER,
      children: [new TextRun({ text, bold: true, size: 20, font: "Arial" })]
    })]
  });
}

function createCell(text, width = 1500, bold = false) {
  return new TableCell({
    borders: cellBorders,
    width: { size: width, type: WidthType.DXA },
    children: [new Paragraph({
      children: [new TextRun({ text, bold, size: 20, font: "Arial" })]
    })]
  });
}

// 创建bullet list配置
const bulletListConfig = {
  reference: "bullet-list",
  levels: [{
    level: 0,
    format: LevelFormat.BULLET,
    text: "•",
    alignment: AlignmentType.LEFT,
    style: { paragraph: { indent: { left: 720, hanging: 360 } } }
  }]
};

// 创建文档
const doc = new Document({
  styles: {
    default: { document: { run: { font: "Arial", size: 24 } } },
    paragraphStyles: [
      {
        id: "Heading1",
        name: "Heading 1",
        basedOn: "Normal",
        next: "Normal",
        quickFormat: true,
        run: { size: 32, bold: true, color: "000000", font: "Arial" },
        paragraph: { spacing: { before: 240, after: 120 }, outlineLevel: 0 }
      },
      {
        id: "Heading2",
        name: "Heading 2",
        basedOn: "Normal",
        next: "Normal",
        quickFormat: true,
        run: { size: 28, bold: true, color: "000000", font: "Arial" },
        paragraph: { spacing: { before: 180, after: 90 }, outlineLevel: 1 }
      },
      {
        id: "Heading3",
        name: "Heading 3",
        basedOn: "Normal",
        next: "Normal",
        quickFormat: true,
        run: { size: 24, bold: true, color: "000000", font: "Arial" },
        paragraph: { spacing: { before: 120, after: 60 }, outlineLevel: 2 }
      }
    ]
  },
  numbering: {
    config: [bulletListConfig]
  },
  sections: [{
    properties: {
      page: {
        margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 }
      }
    },
    children: [
      // 标题
      new Paragraph({
        heading: HeadingLevel.TITLE,
        alignment: AlignmentType.CENTER,
        children: [new TextRun({ text: "PCB布局复用方案评估报告", bold: true, size: 48 })]
      }),
      new Paragraph({ children: [new TextRun("")] }), // 空行

      // 1. 实现目的
      new Paragraph({
        heading: HeadingLevel.HEADING_1,
        children: [new TextRun("1. 实现目的")]
      }),

      // 1.1 项目背景
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("1.1 项目背景")]
      }),
      new Paragraph({
        children: [new TextRun("我们需要实现一个PCB布局复用功能，能够将参考电路单元的布局（包括元件位置、走线、zone等）自动复制到其他相同拓扑的目标单元，以提高设计效率。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 1.2 评估目标
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("1.2 评估目标")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("对比四种布局复用方案的技术特点、实现难度和适用场景")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("系统评估功能覆盖范围、工程可行性和开发成本")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("提供详细的实现步骤和工时分解")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("为技术选型提供依据，选择最适合的实现方案")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 1.3 最终结论
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("1.3 最终结论")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "经过对四种方案的深入评估，推荐", font: "Arial" }),
                   new TextRun({ text: "基于Group的布局复用方案", bold: true }),
                   new TextRun("。")]
      }),
      new Paragraph({
        children: [new TextRun("推荐理由：")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("完全在PCB编辑器中操作，无需原理图介入，工作流简洁")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("实现难度较高（难度等级4），主要挑战在于版本适配和兼容性")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("工时可控（7人天），功能全面")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("功能全面，覆盖完整布局复用流程：拓扑匹配、变换计算、复制执行")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("提供专用UI对话框，操作直观")]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_1,
        children: [new TextRun("2. 方案对比和评估")]
      }),

      // 2.1 总览
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("2.1 总览")]
      }),

      // 方案对比表格
      new Table({
        columnWidths: [1080, 2500, 1170, 1170, 1170, 1170, 1080, 1080],
        margins: { top: 100, bottom: 100, left: 180, right: 180 },
        rows: [
          // 表头
          new TableRow({
            tableHeader: true,
            children: [
              createHeaderCell("编号", 1080),
              createHeaderCell("方案名称", 2500),
              createHeaderCell("正常评估(人天)", 1170),
              createHeaderCell("乐观评估(人天)", 1170),
              createHeaderCell("悲观评估(人天)", 1170),
              createHeaderCell("Delta评估(人天)", 1170),
              createHeaderCell("难度等级", 1080),
              createHeaderCell("是否推荐", 1080)
            ]
          }),
          // 数据行
          new TableRow({
            children: [
              createCell("1", 1080),
              createCell("基于原理图Sheet的布局复用", 2500),
              createCell("8", 1170),
              createCell("6", 1170),
              createCell("11", 1170),
              createCell("8.2", 1170),
              createCell("3", 1080),
              createCell("不推荐", 1080)
            ]
          }),
          new TableRow({
            children: [
              createCell("2", 1080),
              createCell("基于Component Class的布局复用", 2500),
              createCell("9", 1170),
              createCell("7", 1170),
              createCell("12", 1170),
              createCell("9.3", 1170),
              createCell("5", 1080),
              createCell("不推荐", 1080)
            ]
          }),
          new TableRow({
            children: [
              createCell("3", 1080),
              createCell("基于Group的布局复用", 2500, true),
              createCell("7", 1170, true),
              createCell("5", 1170, true),
              createCell("9", 1170, true),
              createCell("7.0", 1170, true),
              createCell("4", 1080, true),
              new TableCell({
                borders: cellBorders,
                width: { size: 1080, type: WidthType.DXA },
                shading: { fill: "D4EDDA", type: ShadingType.CLEAR },
                children: [new Paragraph({
                  children: [new TextRun({ text: "推荐", bold: true, size: 20, font: "Arial" })]
                })]
              })
            ]
          }),
          new TableRow({
            children: [
              createCell("4", 1080),
              createCell("基于位号映射的布局复用", 2500),
              createCell("10", 1170),
              createCell("8", 1170),
              createCell("14", 1170),
              createCell("10.7", 1170),
              createCell("4", 1080),
              createCell("不推荐", 1080)
            ]
          })
        ]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({
        children: [new TextRun({ text: "Delta计算公式: (乐观 + 4×正常 + 悲观) / 6", italics: true, size: 20 })]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 2.2 各方案详细描述
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("2.2 各方案详细描述")]
      }),

      // 2.2.1 方案1
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("2.2.1 基于原理图Sheet的布局复用")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "实现原理：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("通过原理图的层级结构(Sheet hierarchy)识别需要复用的元件。使用memberOfSheetOrChildren()表达式在Rule Area中查询属于特定Sheet的元件。")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "优点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("原理图驱动，适合层级化设计")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("支持Sheet层级关系，可以递归匹配子Sheet")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "缺点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("必须通过原理图定义，无法在PCB中独立操作")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("需要维护原理图和PCB的同步")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("灵活性受原理图设计限制")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "评估：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("该方案设计目标明确，但强依赖原理图层级结构。对于不使用层级化原理图设计的项目，此方案不适用。实现难度适中(难度等级3)。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 2.2.2 方案2
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("2.2.2 基于Component Class的布局复用")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "实现原理：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("通过元器件分类系统识别需要复用的元件。使用hasComponentClass()表达式在Rule Area中查询属于特定类别的元件。支持Atomic class和Composite class组合。")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "优点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("按功能分类，语义清晰")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("支持Composite class，可以组合多个分类")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("分类可以复用，便于管理")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "缺点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("必须通过原理图定义元器件分类")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("需要额外的分类管理工作")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("无法在PCB中独立操作")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "评估：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("该方案提供了良好的语义化分类系统，但需要提前规划和维护元器件分类。实现难度高(难度等级5)，需要额外开发分类管理UI。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({ children: [new TextRun("")] }),

      // 2.2.3 方案3 (推荐)
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("2.2.3 基于Group的布局复用（推荐）")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "实现原理：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("直接在PCB编辑器中通过Group分组识别元件。使用memberOfGroup()表达式在Rule Area中查询属于特定Group的元件。提供专用UI对话框，用户可以可视化选择参考Group并配置复用选项。")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "优点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("完全在PCB编辑器中操作，无需原理图介入")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("灵活性高，可随时调整Group成员")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("提供专用UI对话框，操作直观")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("支持拓扑自动匹配，验证元件连接关系")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("支持嵌套Group，处理复杂层级结构")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "缺点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("需要在PCB中手动创建和维护Group")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("Group信息不在原理图中体现")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "评估：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("推荐使用。该方案实现难度较高(难度等级4)，主要挑战在于版本适配和兼容性处理，但功能全面，覆盖完整的布局复用流程，包括拓扑匹配、变换计算、布局复制等核心功能。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 2.2.4 方案4
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("2.2.4 基于位号映射的布局复用")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "实现原理：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("通过封装位号的前缀映射完成布局复用。例如：U1A→U2A，U1B→U2B，R1_1→R2_1。解析位号，提取前缀和后缀，建立映射关系，自动识别相同拓扑的元件组。")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "优点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("无需Group或原理图配置")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("适合规律命名的设计")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("自动识别，无需手动分组")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "缺点：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("需要严格的命名规范")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("映射关系复杂时容易出错")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("缺乏可视化反馈")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("没有现成实现，需要从零开发")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "评估：", bold: true })]
      }),
      new Paragraph({
        children: [new TextRun("该方案思路简洁，但实现难度高(难度等级4)，工时较长(10人天)。位号解析算法复杂，需要处理多种命名模式。缺少现成参考实现，开发风险较高。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({ children: [new TextRun("")] }),

      // 3. 难点和疑问点
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_1,
        children: [new TextRun("3. 本项目的难点和疑问点")]
      }),

      // 3.1
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("3.1 代码版本差异")]
      }),
      new Paragraph({
        children: [new TextRun("现有代码和参考代码之间存在架构差异，API接口变化频繁，类结构调整较大，需要处理大量兼容性工作。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 3.2
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("3.2 数据结构兼容性")]
      }),
      new Paragraph({
        children: [new TextRun("两个版本的数据结构定义存在差异，需要处理序列化和反序列化兼容性，确保文件格式正确。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 3.3
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("3.3 UI框架兼容性")]
      }),
      new Paragraph({
        children: [new TextRun("两个版本的UI框架可能存在差异，对话框布局和事件处理机制需要适配。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 3.4
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("3.4 测试和验证")]
      }),
      new Paragraph({
        children: [new TextRun("功能适配完成后需要进行全面测试，包括不同设计场景的兼容性测试、边界条件测试和性能测试。")]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({ children: [new TextRun("")] }),

      // 4. 方案选择
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_1,
        children: [new TextRun("4. 方案选择")]
      }),

      // 4.1 选择标准
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("4.1 选择标准")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("功能全面性：覆盖完整的PCB布局复用流程，包括元件识别、拓扑匹配、变换计算、布局复制等")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("工程可行性：实现难度适中，有明确的技术路径，开发风险可控")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("技术先进性：采用现代化算法，性能和精度满足实际需求")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("用户友好性：操作流程简洁，提供良好的交互界面和反馈")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("开发成本：工时合理，符合项目预算和周期要求")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 4.2 推荐方案
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("4.2 推荐方案")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "推荐方案：基于Group的布局复用", bold: true, size: 24 })]
      }),
      new Paragraph({
        children: [new TextRun("该方案直接在PCB编辑器中操作，无需原理图介入，工作流简洁高效。主要优势包括：")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("工作流优势：完全在PCB编辑器完成，无需切换到原理图，提高设计效率")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("实现难度：较高(难度等级4)，主要挑战在于版本适配和兼容性，工时可控(7人天)")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("功能完整：覆盖拓扑匹配、变换计算、复制执行等完整流程")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("用户体验：提供专用UI对话框，支持可视化选择和配置")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("灵活性高：Group成员可随时调整，支持嵌套Group处理复杂层级")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 4.3 不推荐方案
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("4.3 不推荐方案及原因")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "方案1: 基于原理图Sheet的布局复用", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("必须依赖原理图层级结构，无法在PCB中独立操作")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("不适合没有使用层级化原理图设计的项目")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("工时较长(8人天)，且依赖原理图同步")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        children: [new TextRun({ text: "方案2: 基于Component Class的布局复用", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("需要额外的元器件分类管理工作")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("必须通过原理图定义分类，PCB无法独立操作")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("实现难度高(难度等级5)，工时最长(9人天)")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        children: [new TextRun({ text: "方案4: 基于位号映射的布局复用", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("需要严格的命名规范，实际应用受限")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("映射算法复杂，容易出错")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("没有现成实现，需要从零开发，工时最长(10人天)，风险高")]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({ children: [new TextRun("")] }),

      // 5. 详细的实现步骤以及工时分解
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_1,
        children: [new TextRun("5. 详细的实现步骤以及工时分解")]
      }),

      // 5.1 各方案工时汇总表
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("5.1 各方案工时汇总")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 工时汇总表
      new Table({
        columnWidths: [4680, 2340, 2340],
        margins: { top: 100, bottom: 100, left: 180, right: 180 },
        rows: [
          new TableRow({
            tableHeader: true,
            children: [
              createHeaderCell("方案名称", 4680),
              createHeaderCell("工时(人天)", 2340),
              createHeaderCell("工时(人月, 按22天/月)", 2340)
            ]
          }),
          new TableRow({
            children: [
              createCell("基于原理图Sheet的布局复用", 4680),
              createCell("8", 2340),
              createCell("0.36", 2340)
            ]
          }),
          new TableRow({
            children: [
              createCell("基于Component Class的布局复用", 4680),
              createCell("9", 2340),
              createCell("0.41", 2340)
            ]
          }),
          new TableRow({
            children: [
              createCell("基于Group的布局复用（推荐）", 4680, true),
              createCell("7", 2340, true),
              createCell("0.32", 2340, true)
            ]
          }),
          new TableRow({
            children: [
              createCell("基于位号映射的布局复用", 4680),
              createCell("10", 2340),
              createCell("0.45", 2340)
            ]
          })
        ]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 5.2 方案1模块分解
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("5.2 方案1: 基于原理图Sheet的布局复用 - 模块分解")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Table({
        columnWidths: [3744, 1872],
        margins: { top: 100, bottom: 100, left: 180, right: 180 },
        rows: [
          new TableRow({
            tableHeader: true,
            children: [
              createHeaderCell("模块名称", 3744),
              createHeaderCell("工时(人天)", 1872)
            ]
          }),
          new TableRow({ children: [createCell("原理图Sheet系统集成", 3744), createCell("2", 1872)] }),
          new TableRow({ children: [createCell("memberOfSheetOrChildren表达式实现", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("Rule Area查询机制", 3744), createCell("1.5", 1872)] }),
          new TableRow({ children: [createCell("拓扑匹配引擎", 3744), createCell("2", 1872)] }),
          new TableRow({ children: [createCell("布局复用核心逻辑", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("测试和优化", 3744), createCell("0.5", 1872)] }),
          new TableRow({ children: [createCell("总计", 3744, true), createCell("8", 1872, true)] })
        ]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 5.3 方案2模块分解
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("5.3 方案2: 基于Component Class的布局复用 - 模块分解")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Table({
        columnWidths: [3744, 1872],
        margins: { top: 100, bottom: 100, left: 180, right: 180 },
        rows: [
          new TableRow({
            tableHeader: true,
            children: [
              createHeaderCell("模块名称", 3744),
              createHeaderCell("工时(人天)", 1872)
            ]
          }),
          new TableRow({ children: [createCell("Component Class系统集成", 3744), createCell("2.5", 1872)] }),
          new TableRow({ children: [createCell("hasComponentClass表达式实现", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("分类管理UI", 3744), createCell("1.5", 1872)] }),
          new TableRow({ children: [createCell("Rule Area查询机制", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("拓扑匹配引擎", 3744), createCell("2", 1872)] }),
          new TableRow({ children: [createCell("布局复用核心逻辑", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("总计", 3744, true), createCell("9", 1872, true)] })
        ]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({ children: [new TextRun("")] }),

      // 5.4 方案3模块分解（推荐方案，详细分解）
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("5.4 方案3: 基于Group的布局复用 - 详细模块分解（推荐）")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Table({
        columnWidths: [3744, 1872],
        margins: { top: 100, bottom: 100, left: 180, right: 180 },
        rows: [
          new TableRow({
            tableHeader: true,
            children: [
              createHeaderCell("模块名称", 3744),
              createHeaderCell("工时(人天)", 1872)
            ]
          }),
          new TableRow({ children: [createCell("Group系统集成", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("memberOfGroup表达式实现", 3744), createCell("0.5", 1872)] }),
          new TableRow({ children: [createCell("Group布局复用对话框", 3744), createCell("1.5", 1872)] }),
          new TableRow({ children: [createCell("Rule Area自动生成", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("拓扑匹配引擎", 3744), createCell("2", 1872)] }),
          new TableRow({ children: [createCell("布局复用核心逻辑", 3744), createCell("1", 1872)] }),
          new TableRow({ children: [createCell("总计", 3744, true), createCell("7", 1872, true)] })
        ]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 详细的模块说明
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("5.4.1 模块1: Group系统集成")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要工作: 集成Group管理功能，支持Group创建、编辑和查询")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("估计工时: 1人天")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("风险等级: 低")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要风险点: Group API兼容性")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("5.4.2 模块2: memberOfGroup表达式实现")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要工作: 实现PCB表达式函数，用于Rule Area查询Group成员")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("估计工时: 0.5人天")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("风险等级: 低")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要风险点: 表达式解析兼容性")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("5.4.3 模块3: Group布局复用对话框")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要工作: 设计和实现专用UI对话框，显示Group列表，配置复用选项")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("估计工时: 1.5人天")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("风险等级: 中")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要风险点: UI交互设计复杂度")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("5.4.4 模块4: Rule Area自动生成")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要工作: 从Group自动生成Rule Area zone，计算包围框，配置属性")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("估计工时: 1人天")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("风险等级: 中")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要风险点: 包围框计算精度")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("5.4.5 模块5: 拓扑匹配引擎")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要工作: 实现图同构算法，构建元件连接图，验证管脚拓扑")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("估计工时: 2人天")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("风险等级: 高")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要风险点: 算法复杂度、回溯搜索性能")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("5.4.6 模块6: 布局复用核心逻辑")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要工作: 计算变换矩阵，复制元件位置、走线、过孔等")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("估计工时: 1人天")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("风险等级: 中")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("主要风险点: 变换精度、网络映射错误")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 5.5 方案4模块分解
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("5.5 方案4: 基于位号映射的布局复用 - 模块分解")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Table({
        columnWidths: [3744, 1872],
        margins: { top: 100, bottom: 100, left: 180, right: 180 },
        rows: [
          new TableRow({
            tableHeader: true,
            children: [
              createHeaderCell("模块名称", 3744),
              createHeaderCell("工时(人天)", 1872)
            ]
          }),
          new TableRow({ children: [createCell("位号解析和映射算法", 3744), createCell("2.5", 1872)] }),
          new TableRow({ children: [createCell("映射规则配置UI", 3744), createCell("2", 1872)] }),
          new TableRow({ children: [createCell("映射验证机制", 3744), createCell("1.5", 1872)] }),
          new TableRow({ children: [createCell("拓扑匹配引擎", 3744), createCell("2", 1872)] }),
          new TableRow({ children: [createCell("布局复用核心逻辑", 3744), createCell("1.5", 1872)] }),
          new TableRow({ children: [createCell("测试和错误处理", 3744), createCell("0.5", 1872)] }),
          new TableRow({ children: [createCell("总计", 3744, true), createCell("10", 1872, true)] })
        ]
      }),
      new Paragraph({ children: [new TextRun("")] }),
      new Paragraph({ children: [new TextRun("")] }),

      // 6. 架构以及实施步骤
      new Paragraph({
        pageBreakBefore: true,
        heading: HeadingLevel.HEADING_1,
        children: [new TextRun("6. 架构以及实施步骤")]
      }),

      // 6.1 算法总体流程
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("6.1 算法总体流程")]
      }),
      new Paragraph({
        children: [new TextRun("基于Group的PCB布局复用算法分为以下五个核心阶段：")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("阶段1: Group识别和Rule Area生成 - 从PCB编辑器中选择参考Group，自动生成Rule Area")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("阶段2: 拓扑匹配 - 构建连接图，使用图同构算法匹配参考Group和目标Group")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("阶段3: 变换计算 - 计算参考区域到目标区域的几何变换（位移和旋转）")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("阶段4: 布局复用执行 - 应用变换，复制元件位置、走线、zone等PCB对象")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("阶段5: 验证和优化 - 检查网络连接、DRC验证、性能优化")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 6.2 实施步骤详解
      new Paragraph({
        heading: HeadingLevel.HEADING_2,
        children: [new TextRun("6.2 实施步骤详解")]
      }),

      // 阶段1
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("6.2.1 阶段1: Group识别和Rule Area生成")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "目标：", bold: true }), new TextRun("建立Group到Rule Area的映射关系，为后续拓扑匹配做准备")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输入：", bold: true }), new TextRun("PCB设计文件，包含多个Group")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输出：", bold: true }), new TextRun("Rule Area列表，每个Rule Area关联一个Group")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "关键步骤：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("遍历PCB中所有Group对象")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("计算每个Group的包围框（bounding box）")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("创建ZONE对象，配置为Rule Area模式")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("设置Rule Area属性：m_isRuleArea=true, m_placementAreaEnabled=true")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("配置PCBEXPR查询表达式：memberOfGroup(group_name)")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 阶段2
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("6.2.2 阶段2: 拓扑匹配")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "目标：", bold: true }), new TextRun("验证参考Group和目标Group具有相同的电路拓扑结构")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输入：", bold: true }), new TextRun("参考Rule Area和目标Rule Area")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输出：", bold: true }), new TextRun("元件映射关系表（参考元件 → 目标元件）")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "关键步骤：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("构建连接图：使用TMATCH命名空间的CONNECTION_GRAPH类")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("图节点：COMPONENT对象（元件），每个节点包含管脚列表")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("图边：连接关系（通过net连接的管脚对）")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("图同构算法：回溯搜索，迭代限制10000次")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("验证条件：元件类型匹配、管脚数匹配、网络连接匹配")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 阶段3
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("6.2.3 阶段3: 变换计算")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "目标：", bold: true }), new TextRun("计算参考区域到目标区域的几何变换矩阵")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输入：", bold: true }), new TextRun("参考Rule Area和目标Rule Area，元件映射关系")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输出：", bold: true }), new TextRun("变换矩阵（位移向量 + 旋转角度）")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "关键步骤：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("选择Anchor元件（通常选择参考Group中的第一个元件）")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("计算位移向量：目标Anchor位置 - 参考Anchor位置")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("计算旋转角度：比较Anchor元件的朝向差异")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("构建变换矩阵：使用内置的向量和角度类")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 阶段4
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("6.2.4 阶段4: 布局复用执行")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "目标：", bold: true }), new TextRun("将参考布局应用到目标区域")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输入：", bold: true }), new TextRun("变换矩阵、元件映射关系、复用选项")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输出：", bold: true }), new TextRun("更新后的PCB设计")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "关键步骤：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("复制元件位置：遍历映射表，对每个目标元件应用变换")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("复制走线：查找参考区域的PCB_TRACK对象，复制到目标区域")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("复制过孔：查找参考区域的PCB_VIA对象，复制到目标区域")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("复制Zone：查找参考区域的ZONE对象，复制到目标区域")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("网络代码修正：更新复制对象的net code，映射到目标网络")]
      }),
      new Paragraph({ children: [new TextRun("")] }),

      // 阶段5
      new Paragraph({
        heading: HeadingLevel.HEADING_3,
        children: [new TextRun("6.2.5 阶段5: 验证和优化")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "目标：", bold: true }), new TextRun("确保复用后的布局正确且性能可接受")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输入：", bold: true }), new TextRun("更新后的PCB设计")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "输出：", bold: true }), new TextRun("验证报告")]
      }),
      new Paragraph({
        children: [new TextRun({ text: "关键步骤：", bold: true })]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("网络连接检查：验证所有复制的走线正确连接到目标元件")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("DRC验证：运行设计规则检查，确保无间距错误")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("性能测试：记录拓扑匹配和复制操作的耗时")]
      }),
      new Paragraph({
        numbering: { reference: "bullet-list", level: 0 },
        children: [new TextRun("优化建议：如果性能不满足要求，考虑使用空间索引或并行计算")]
      }),
      new Paragraph({ children: [new TextRun("")] })
    ]
  }]
});

// 保存文档
Packer.toBuffer(doc).then(buffer => {
  fs.writeFileSync("PCB-Group-Layout-Reuse-Plan.docx", buffer);
  console.log("文档创建成功: PCB-Group-Layout-Reuse-Plan.docx");
}).catch(err => {
  console.error("文档创建失败:", err);
});
