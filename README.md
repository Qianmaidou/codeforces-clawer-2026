# 使用说明

## 编译

gcc -o main.exe main.c curl_part.c cjson_part.c html_part.c cJSON.c -lcurl

## 运行

./main.exe

## 重要：必须附带模板文件

程序生成报告时依赖 template/template.html，***\*缺少此文件程序将无法工作\****。

正确的目录结构：

Codeforces_Clawer/
├── main.exe
├── template/
│  └── template.html   ← 必须存在！
└── out/         ← 自动创建

分发 main.exe 给他人使用时，***\*必须连同\**** ***\*template/\**** ***\*文件夹一起复制\****，否则运行时会报错：
Error: template not found

## 使用方法

1. 运行程序，菜单输入 1 选择用户查询
2. 输入 Codeforces handle（如 tourist）
3. 等待数据抓取和报告生成
4. 用浏览器打开 out/ 目录下的 HTML 文件查看报告
5. 菜单输入2 退出程序