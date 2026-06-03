<!--
══════════════════════════════════════════════════════════════
Codeforces 用户数据报告模板
作用：生成一个包含用户信息、等级分曲线、题目分布和比赛详情的HTML报告页面
使用方式：后端爬虫将 {{DATA}} 替换为实际的 JSON 数据后输出此HTML
依赖库：ECharts 5.5.0（CDN加载），用于绘制图表
══════════════════════════════════════════════════════════════
-->
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<!-- 声明文档编码为UTF-8，确保中文字符正常显示 -->
<meta charset="UTF-8">
<!-- viewport 设置：适配移动端，初始缩放1.0 -->
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Codeforces 用户报告</title>
<!-- 引入 ECharts 图表库，用于绘制等级分曲线和直方图 -->
<script src="https://cdn.jsdelivr.net/npm/echarts@5.5.0/dist/echarts.min.js"></script>
<style>
/* ────────── 全局页面样式 ────────── */
body {
    /* 字体：优先Arial，中文字体回退到微软雅黑 */
    font-family: Arial, 'Microsoft YaHei', sans-serif;
    background: #fff; color: #333;
    /* 页面居中，最大宽度1000px */
    max-width: 1000px; margin: 0 auto; padding: 20px;
}

/* ────────── 标题样式 ────────── */
h1 { font-size: 22px; margin: 0 0 4px; }
h2 {
    font-size: 16px; margin: 24px 0 12px;
    /* 左侧蓝色竖线装饰，用于区分各个板块 */
    padding-left: 8px; border-left: 3px solid #06c;
}

/* ────────── 用户信息区域 ────────── */
.user-info { overflow: hidden; margin-bottom: 20px; }
/* 用户头像：左浮动，圆形裁剪 64x64 */
.user-info img {
    float: left; width: 64px; height: 64px;
    border-radius: 50%; margin-right: 16px;
}
/* 用户详情文字：左浮动，与头像并排 */
.user-info .detail { float: left; }
/* 等级称号样式（如"国际大师"） */
.user-info .rank { color: #666; font-size: 18px; }
/* 用户名和Rating在同一行，flex布局对齐基线 */
.user-info .name-row {
    display: flex; align-items: baseline; gap: 16px; flex-wrap: wrap;
}

/* ────────── 统计卡片区域（浮动四列布局） ────────── */
.stats { overflow: hidden; margin-bottom: 20px; }
.stat-item {
    float: left; width: 25%;           /* 四列等宽 */
    text-align: center; padding: 12px 0;
    border: 1px solid #e0e0e0;         /* 浅灰边框 */
    box-sizing: border-box;            /* 边框计入宽度，避免溢出 */
}
.stat-item .num { font-size: 24px; font-weight: bold; }  /* 统计数值 */
.stat-item .lbl { font-size: 12px; color: #888; margin-top: 2px; } /* 统计标签 */

/* ────────── 图表容器 ────────── */
.chart-box { margin-bottom: 24px; }
.chart-box .title { font-size: 16px; font-weight: bold; margin-bottom: 8px; }
.chart { width: 100%; height: 400px; } /* ECharts 图表需显式设置宽高 */

/* ────────── 选项卡（Tab切换）按钮 ────────── */
.tabs { margin-bottom: 8px; }
.tabs button {
    padding: 4px 14px; margin-right: 4px;
    border: 1px solid #ccc; background: #f8f8f8; color: #333;
    cursor: pointer; font-size: 13px; border-radius: 3px;
}
/* 当前选中的选项卡：蓝色高亮 */
.tabs button.active { background: #06c; color: #fff; border-color: #06c; }

/* ────────── 表格样式 ────────── */
table { width: 100%; border-collapse: collapse; font-size: 13px; }
/* 表头：浅灰背景，底部双线分隔 */
th {
    background: #f0f0f0; padding: 8px 6px;
    text-align: left; border-bottom: 2px solid #ccc;
}
/* 单元格：底部细线分隔，内容垂直居中 */
td { padding: 6px; border-bottom: 1px solid #eee; vertical-align: middle; }
/* 鼠标悬停行：浅灰高亮 */
tr:hover td { background: #fafafa; }

/* ────────── 题目状态小方块 ────────── */
.prob {
    display: inline-block; width: 32px; height: 32px;
    line-height: 32px;           /* 垂直居中文字 */
    text-align: center; margin: 1px;
    border-radius: 4px; font-size: 12px; font-weight: bold;
}
/* 赛中通过：绿色 */
.solved-in    { background: #4caf50; color: #fff; }
/* 赛后补题通过：橙色 */
.solved-after { background: #ff9800; color: #fff; }
/* 未通过：灰色 */
.not-solved   { background: #e0e0e0; color: #999; }

/* ────────── 图例样式 ────────── */
.legend { font-size: 12px; margin-bottom: 8px; }
.legend span { margin-right: 16px; }
.legend .box {
    display: inline-block; width: 14px; height: 14px;
    border-radius: 2px; vertical-align: middle; margin-right: 4px;
}

/* ────────── 页脚 ────────── */
.footer { text-align: center; color: #aaa; font-size: 12px; margin-top: 30px; }
</style>
</head>
<body>

<!--
══════════════════════════════════════════
1. 用户信息区域
   展示头像、用户名（首字母可能有特殊颜色）、Rating、等级称号
   数据由 JS 动态填充
══════════════════════════════════════════
-->
<div class="user-info">
    <!-- 用户头像，src 由 JS 设置 -->
    <img id="avatar" src="" alt="">
    <div class="detail">
        <div class="name-row">
            <!-- 用户名（含首字母特殊颜色） -->
            <span style="font-size:22px;font-weight:bold;" id="handle-name"></span>
            <!-- 当前Rating分值 -->
            <span style="font-size:20px;font-weight:bold;" id="rating-text"></span>
        </div>
        <!-- 等级称号，如"国际大师"、"专家" -->
        <div class="rank" id="rank-text"></div>
    </div>
    <!-- 清除浮动，使父容器能包裹浮动子元素 -->
    <div style="clear:both"></div>
</div>

<!--
══════════════════════════════════════════
2. 统计卡片区域（四列）
   - 比赛次数
   - 最高等级分
   - 近180天比赛数
   - 近180天最高分
══════════════════════════════════════════
-->
<div class="stats">
    <div class="stat-item"><div class="num" id="s-contests">-</div><div class="lbl">比赛次数</div></div>
    <div class="stat-item"><div class="num" id="s-maxr">-</div><div class="lbl">最高等级分</div></div>
    <div class="stat-item"><div class="num" id="s-r180c">-</div><div class="lbl">近180天比赛</div></div>
    <div class="stat-item"><div class="num" id="s-r180m">-</div><div class="lbl">近180天最高分</div></div>
</div>

<!--
══════════════════════════════════════════
3. 等级分变化曲线图（ECharts 折线图）
   横轴：比赛日期，纵轴：Rating 分值
   每个数据点颜色根据 Rating 等级变化
══════════════════════════════════════════
-->
<div class="chart-box">
    <div class="title">等级分变化曲线</div>
    <!-- ECharts 会将图表渲染到此 div 内 -->
    <div class="chart" id="chart-rating"></div>
</div>

<!--
══════════════════════════════════════════
4. 通过的题目等级分分布图（ECharts 柱状图）
   按分数段统计通过的题目数量
   支持四个时间范围切换：全部/近一年/近180天/近一月
══════════════════════════════════════════
-->
<div class="chart-box">
    <div class="title">通过的题目等级分分布</div>
    <!-- 时间范围切换按钮 -->
    <div class="tabs">
        <button class="active" onclick="switchDistTab('all')">全部</button>
        <button onclick="switchDistTab('year')">近一年</button>
        <button onclick="switchDistTab('halfYear')">近180天</button>
        <button onclick="switchDistTab('month')">近一月</button>
    </div>
    <div class="chart" id="chart-dist"></div>
</div>

<!--
══════════════════════════════════════════
5. 比赛详情表格
   每行一场比赛，显示：序号、名称、时间、赛前分、
   赛后分、排名、赛中通过题目、赛后补题
   表格内容由 JS 动态生成
══════════════════════════════════════════
-->
<div class="chart-box">
    <div class="title">比赛详情（最新在前）</div>
    <!-- 图例：说明颜色含义 -->
    <div class="legend">
        <span><span class="box solved-in"></span> 赛中通过</span>
        <span><span class="box solved-after"></span> 赛后补题</span>
    </div>
    <!-- 最大高度600px，超出滚动 -->
    <div style="max-height:600px;overflow-y:auto;">
        <table>
            <thead>
                <tr>
                    <th>#</th><th>赛事名称</th><th>时间</th>
                    <th>赛前分</th><th>赛后分</th><th>排名</th><th>赛中通过</th><th>补题</th>
                </tr>
            </thead>
            <!-- 比赛数据由 JS 动态生成行 -->
            <tbody id="contest-tbody"></tbody>
        </table>
    </div>
</div>

<!-- 页脚：声明数据来源 -->
<div class="footer">Codeforces Crawler 2026 &nbsp;|&nbsp; 数据来源: codeforces.com/api</div>

<script>
/*
══════════════════════════════════════════════════════════════
{{DATA}} 是模板占位符，后端会将其替换为如下结构的 JSON 赋值语句：
  var USER_DATA = { ... };
包含字段：
  - handle:         用户名
  - handleColor:    用户名颜色（可选，按Rating着色）
  - specialHandle:  是否为特殊用户名（如Tourist的首字母有特殊色）
  - firstLetterColor: 用户名字母颜色
  - avatar:         头像URL
  - rating:         当前Rating
  - rank:           等级称号
  - ratingHistory:  [{time, name, oldRating, newRating, rank}, ...]  等级分历史
  - problemRatingDist: { all:[], year:[], halfYear:[], month:[], unrated:[] }  各时间段题目分布
  - stats:          { contestCount, maxRating, recent180Contests, recent180MaxRating }
  - contests:       [{time, name, oldRating, newRating, rank, problems:[{index,rating,inContest,afterContest}]}, ...]
══════════════════════════════════════════════════════════════
*/
{{DATA}}

/*
 * 根据 Rating 分值返回对应的颜色
 * Codeforces 的 Rating 颜色体系：
 *   ≥4000  Touris黑红   ≥3000  Legendary Grandmaster 黑红
 *   ≥2600  International Grandmaster 红
 *   ≥2400  Grandmaster 红      ≥2300  International Master 橙
 *   ≥2100  Master 橙           ≥1900  Candidate Master 紫
 *   ≥1600  Expert 蓝           ≥1400  Specialist 青
 *   ≥1200  Pupil 绿            >0     Newbie 灰
 *   0       未评级 黑
 */
function getRatingColor(r) {
    if (r >= 4000) return '#FF0000'; if (r >= 3000) return '#FF0000';
    if (r >= 2600) return '#FF0000'; if (r >= 2400) return '#FF0000';
    if (r >= 2300) return '#FF8C00'; if (r >= 2100) return '#FF8C00';
    if (r >= 1900) return '#AA00AA'; if (r >= 1600) return '#0000FF';
    if (r >= 1400) return '#03A89E'; if (r >= 1200) return '#008000';
    if (r >  0)    return '#808080'; return '#000000';
}

/*
 * 立即执行函数（IIFE）：页面加载后立即渲染所有内容
 * 所有变量包裹在函数作用域内，避免污染全局命名空间
 */
(function() {
    // 从后端注入的全局变量获取用户数据
    var d = USER_DATA;

    /* ═══════════ 1. 填充用户信息 ═══════════ */
    // 头像：若未提供则使用 CF 默认头像
    document.getElementById('avatar').src = d.avatar || 'https://userpic.codeforces.com/no-avatar.jpg';

    var h = document.getElementById('handle-name');
    // 用户名颜色：优先使用后端指定的颜色，否则根据Rating自动计算
    var hc = d.handleColor || getRatingColor(d.rating || 0);

    // 特殊用户名处理：首字母使用 firstLetterColor，其余字母使用 handleColor
    // 例如 CF 上某些用户（如Tourist）首字母有特殊标识色
    if (d.specialHandle && d.firstLetterColor && d.handle.length > 0) {
        h.innerHTML = '<span style="color:' + d.firstLetterColor + '">' + d.handle[0] + '</span>'
                    + '<span style="color:' + hc + '">' + d.handle.slice(1) + '</span>';
    } else {
        // 普通用户：整个用户名统一颜色
        h.textContent = d.handle; h.style.color = hc;
    }

    // 等级称号（如 "International Grandmaster"）
    document.getElementById('rank-text').textContent = d.rank || 'Unrated';

    // Rating 显示文本和颜色
    var rt = document.getElementById('rating-text');
    rt.textContent = 'Rating: ' + (d.rating || 0);
    rt.style.color = hc;

    /* ═══════════ 2. 填充统计卡片 ═══════════ */
    var s = d.stats;
    document.getElementById('s-contests').textContent = s.contestCount;       // 总比赛次数
    document.getElementById('s-maxr').textContent    = s.maxRating;           // 历史最高Rating
    document.getElementById('s-r180c').textContent   = s.recent180Contests;   // 近180天参赛数
    document.getElementById('s-r180m').textContent   = s.recent180MaxRating;  // 近180天最高Rating

    /* ═══════════ 3. 渲染等级分变化曲线（ECharts 折线图） ═══════════ */
    var chartRating = echarts.init(document.getElementById('chart-rating'));
    var hist = d.ratingHistory;
    var dates = [], ratings = [];

    // 遍历历史数据，构建横轴日期数组和纵轴数据数组
    // 每条数据格式为 [索引, 新Rating]，索引用于tooltip回查详细信息
    for (var i = 0; i < hist.length; i++) {
        // 将Unix时间戳转为本地日期字符串（中文格式）
        dates.push(new Date(hist[i].time * 1000).toLocaleDateString('zh-CN'));
        ratings.push([i, hist[i].newRating]);
    }

    // ECharts 配置项
    chartRating.setOption({
        // 提示框：鼠标悬停时显示比赛名称、日期、Rating变化、排名
        tooltip: { trigger: 'axis', formatter: function(p) {
            var idx = p[0].dataIndex, dd = hist[idx];
            return '<b>' + dd.name + '</b><br/>'        // 比赛名称（加粗）
                   + dates[idx] + '<br/>'                // 比赛日期
                   + 'Rating: ' + dd.newRating + ' (#' + dd.rank + ')<br/>'  // 新Rating和排名
                   + dd.oldRating + ' → ' + dd.newRating; // Rating变化
        }},
        // 绘图网格：左留60px给Y轴标签，右留20px，上留16px，下留48px给旋转的X轴标签
        grid: { left: 60, right: 20, top: 16, bottom: 48 },
        // X轴：类目轴，显示日期，标签旋转45度避免重叠
        xAxis: { type: 'category', data: dates,
            axisLabel: { rotate: 45, fontSize: 9, color: '#666' } },
        // Y轴：数值轴，Rating分值
        yAxis: { type: 'value',
            axisLabel: { color: '#666' }, splitLine: { lineStyle: { color: '#eee' } } },
        series: [{
            type: 'line', data: ratings, smooth: true,   // 平滑折线图
            lineStyle: { color: '#06c', width: 2 },       // 蓝色线条，宽度2px
            // 每个数据点的颜色根据该点的Rating值动态着色
            itemStyle: { color: function(p) { return getRatingColor(p.value[1]); } },
            symbol: 'circle', symbolSize: 3               // 数据点：小圆点
        }]
    });

    // 监听窗口大小变化，自动调整图表尺寸
    window.addEventListener('resize', function() { chartRating.resize(); });

    /* ═══════════ 4. 渲染题目等级分分布图（ECharts 柱状图） ═══════════ */
    var distChart = echarts.init(document.getElementById('chart-dist'));

    /*
     * 根据选择的时间范围渲染分布图
     * range 可选值: 'all' | 'year' | 'halfYear' | 'month'
     * 数据来源于 USER_DATA.problemRatingDist
     */
    function renderDist(range) {
        // 获取对应时间范围的分布数组，默认使用全部数据
        var buckets = d.problemRatingDist[range] || d.problemRatingDist.all;
        // 时间范围索引映射：all→0, year→1, halfYear→2, month→3
        var ri = {all:0, year:1, halfYear:2, month:3}[range] || 0;
        // 未评级（Unrated）题目数量单独提取
        var unratedVal = (d.problemRatingDist.unrated || [0,0,0,0])[ri];

        var xData = [], yData = [];
        // 第一个柱子：未评级题目
        xData.push('Unrated');
        yData.push(unratedVal);

        // 从索引8开始（即800分起），每100分为一个区间
        // buckets[i] 表示分数段 [i*100, (i+1)*100-1] 的通过题目数
        for (var i = 8; i < buckets.length; i++) {
            xData.push((i * 100) + '-' + ((i + 1) * 100 - 1));
            yData.push(buckets[i]);
        }

        distChart.setOption({
            tooltip: { trigger: 'axis' },
            grid: { left: 60, right: 20, top: 8, bottom: 48 },
            // X轴：分数段标签，旋转45度
            xAxis: { type: 'category', data: xData,
                axisLabel: { rotate: 45, fontSize: 9, color: '#666' } },
            yAxis: { type: 'value',
                axisLabel: { color: '#666' }, splitLine: { lineStyle: { color: '#eee' } } },
            series: [{
                type: 'bar', data: yData,
                // 每个柱子颜色根据该分数段中间值对应的Rating等级着色
                itemStyle: { color: function(p) {
                    // 第一个柱子（Unrated）固定灰色
                    if (p.dataIndex === 0) return '#808080';
                    // 取分数段中间值（如"800-899"→850）映射颜色
                    var mid = parseInt(xData[p.dataIndex].split('-')[0]) + 50;
                    return getRatingColor(mid);
                }}
            }]
        });
    }

    // 初始默认显示"全部"时间范围
    renderDist('all');

    window.addEventListener('resize', function() { distChart.resize(); });

    /*
     * 切换分布图的时间范围
     * 由 HTML 中选项卡按钮的 onclick 调用
     * 1) 移除所有按钮的 active 样式
     * 2) 给当前点击的按钮添加 active 样式
     * 3) 重新渲染分布图
     */
    window.switchDistTab = function(range) {
        var btns = document.querySelectorAll('.tabs button');
        btns.forEach(function(b) { b.classList.remove('active'); });
        event.target.classList.add('active');
        renderDist(range);
    };

    /* ═══════════ 5. 渲染比赛详情表格 ═══════════ */
    var html = '';
    var contests = d.contests;

    // 遍历每场比赛（后端已按时间倒序排列）
    for (var i = 0; i < contests.length; i++) {
        var c = contests[i];

        // 格式化日期：Unix时间戳 → YYYY-MM-DD
        var date = new Date(c.time * 1000);
        var ds = date.getFullYear() + '-' +
            String(date.getMonth() + 1).padStart(2, '0') + '-' +
            String(date.getDate()).padStart(2, '0');

        // Rating变化值，正数前加+号
        var delta = c.newRating - c.oldRating;
        var deltaStr = (delta >= 0 ? '+' : '') + delta;

        // 构建赛中通过和赛后补题的HTML
        var solvedHtml = '', practiceHtml = '';
        if (c.problems && c.problems.length > 0) {
            for (var p = 0; p < c.problems.length; p++) {
                var pr = c.problems[p];
                if (pr.inContest) {
                    // 赛中通过：绿色方块，title悬停显示题目编号和分数
                    solvedHtml += '<span class="prob solved-in" title="' +
                        pr.index + ' (' + (pr.rating || '?') + ')">' + pr.index + '</span>';
                } else if (pr.afterContest) {
                    // 赛后补题：橙色方块
                    practiceHtml += '<span class="prob solved-after" title="' +
                        pr.index + ' (' + (pr.rating || '?') + ')">' + pr.index + '</span>';
                }
            }
        }
        // 无通过/无补题时显示灰色横线占位
        if (!solvedHtml) solvedHtml = '<span style="color:#bbb;font-size:12px;">-</span>';
        if (!practiceHtml) practiceHtml = '<span style="color:#bbb;font-size:12px;">-</span>';

        // 拼接表格行
        html += '<tr>' +
            '<td>' + (i + 1) + '</td>' +                                                       // 序号
            // 比赛名称：限制最大宽度，超出显示省略号，title属性显示完整名称
            '<td style="max-width:260px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;" title="' +
                c.name.replace(/"/g, '&quot;') + '">' + c.name + '</td>' +
            '<td style="white-space:nowrap;">' + ds + '</td>' +                                 // 日期，不换行
            // 赛前Rating，颜色按Rating等级
            '<td style="color:' + getRatingColor(c.oldRating) + ';font-weight:bold;">' + c.oldRating + '</td>' +
            // 赛后Rating + 变化量（涨分绿色，掉分红色）
            '<td><span style="color:' + getRatingColor(c.newRating) + ';font-weight:bold;">' +
                c.newRating + '</span> ' +
                '<span style="font-size:11px;color:' + (delta >= 0 ? '#4caf50' : '#e53935') + ';">' +
                deltaStr + '</span></td>' +
            '<td>' + (c.rank || '-') + '</td>' +                                                // 排名
            '<td>' + solvedHtml + '</td>' +                                                     // 赛中通过题目
            '<td>' + practiceHtml + '</td>' +                                                   // 赛后补题
            '</tr>';
    }

    // 将生成的表格行HTML一次性插入tbody
    document.getElementById('contest-tbody').innerHTML = html;
})();
</script>
</body>
</html>
