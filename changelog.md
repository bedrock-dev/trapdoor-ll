### trapdoor-ll-0.17-1.19.10.03

#### 依赖和版本支持

- 需要LiteLoader 2.5.0 或以上版本
- 理论上支持1.19.20.02及以上版本

#### 更新日志

- 支持1.19.20.02 by @dreamguxiang
- 添加`trapdoor crash`命令支持指令崩服
- 添加`trapdoor reload`以支持配置文件热重载(不可重载命令部分)
- 给`data redstone signal`添加更多的信息显示(信号源的dampening,directlyPowered等)
- 修复`log os`可能显示错误的CPU占用信息的bug(#11)
- 给`tick`指令添加部分短命令以兼容旧版命令
- 给shortcuts添加新的触发器`destroy`,可在玩家破坏方块时触发
- 修复自动工具选择的耐久阈值设置错误的问题
- 修复部分拼写错误

### trapdoor-ll-0.16-1.19.10.03

#### 依赖和版本支持

- 需要LiteLoader 2.4.1 或以上版本
- 理论上支持1.19.10及以上版本

#### 更新日志

- 修复无法保存中文名的假人的数据的问题(控制台频繁输出`can not write file`)
- 假人在服务器重启后能自动加回到服务器
- 添加配置项`keep-sim-player-inv`以配置是否保存假人背包数据
- 添加配置项`tool-damage-threshold`以修改自动切换工具的耐久阈值
- 强化`data redstone`指令以查看信号源的连接情况
- 修复少量单词拼写错误

### trapdoor-ll-0.15-1.19.10.03

#### 依赖和版本支持

- 需要LiteLoader 2.4.1 或以上版本
- 理论上支持1.19.10及以上版本

#### 更新日志

- 支持县人装转方块,可使用`trapdoor blockrotate`命令开启,感谢@OEOTYAN的PR
- 支持史莱姆区块显示，可使用`slime`命令配置，感谢@quizhizhe的PR
- 添加计算距离的`dis`命令

### trapdoor-ll-0.14-1.19.10.03

#### 依赖和版本支持

- 需要LiteLoader 2.4.1 或以上版本
- 理论上支持1.19.10及以上版本

#### 更新日志

- 增加 `seed`命令以查看世界种子和玩家附魔种子
- 将`player destroy`拆分成份`player destroyon`和`player destroy`命令，
  前者需要指定方块坐标，后者假人 会自动挖掘其指向的方块
- `hud`添加`chunk`子命令以支持区块显示

### trapdoor-ll-0.13-1.19.10.03

#### 依赖和版本支持

- 需要LiteLoader 2.4.0 或以上版本
- 理论上支持1.19.10及以上版本

#### 更新日志

- 支持1.19.10
- 现在可以在配置文件设置`tweak`默认是否开启

### trapdoor-ll-0.13-1.19.1.01

#### 依赖和版本支持

- 需要LiteLoader 2.3.1 或以上版本
- 理论上支持所有1.19.10之前的版本，**不包括1.19.10**

#### 更新日志

- 添加`log os`命令以显示服务器的内存和CPU占用
- `porf`会默认执行`prof normal 100`
- 现在绝大部分命令都能在命令方块或者后台执行了
- 修复`hsa`无法显示掠夺者前哨站的bug

### trapdoor-ll-0.12-1.19.1.01

这是一个测试`CI/CD`的版本，只修复了少量bug

#### 依赖和版本支持

- 需要LiteLoader 2.3.1 或以上版本
- 理论上支持1.19.x的所有版本

#### 更新日志

- `player`命令现在可以对名字进行补全
- `village`显示村庄列表时会顺序显示
- 简化自动工具切换的代码，提高准确度和效率

### trapdoor-ll-1.19.1.01-beta-12

**需要LL 2.3.1作为支持**

- 修复假人生成位置不准确的问题
- 修复假人生成维度不对的问题
- 当不指定坐标时假人会挖掘它视线所指的方块
- 给hud添加了全局开关以及数据刷新速度设置
- 给例子效果添加性能调节选项(见trapdoor命令)
- 添加发射器/投掷器物品不消耗的功能(创造服)
- 添加强制放方块的功能(创造服)
- 添加无视阻碍打开容器的功能
- 添加挖掘自动换工具的功能(比较OP，大概率还有一些bug)

### trapdoor-ll-1.19.1.01-beta-11

**需要LL 2.3.1作为支持**

- 修复假人的大量bug，包括数据不匹配，崩服，部分指令无效等等
- 给`player`添加`set`子命令，用于设置手中物品(会自动在背包中查询)
- 现在假人死亡后会自动离开游戏
- 给实体/物品名字加上多语言支持(如果需要中文则需要在服务配置文件(server.properties)加上`language='zh_CN'`)
- 优化`spawn analyze print`的输出格式，使其更加易用

### trapdoor-ll-1.19.1.01-beta-10

- 支持1.19.1.01
- 现在配置文件需要放到`plugins/trapdoor/`目录下

### trapdoor-ll-1.18.33-beta-9

- 添加spawn analyze命令，分析刷怪生成
- 添加spawn count命令以统计实体个数
- HUD 添加`add redstone`子命令，用于显示红石原件的强度
- HUD 取消了指向方块名称的显示，添加了所在群系名字的显示
- `data`命令能正常打印方块数据了
- `data`新增加了 `[nbt] [path]`子命令以打印部分数据

### trapdoor-ll-1.18.33-beta-8

- 修复部分拼写错误
- 添加自定义修改快捷键功能(use-on command use)

### trapdoor-ll-1.18.33-beta-7

- 修复 tick acc无法使用的bug
- 修复 hud上指向的方块名字多了一个冒号的bug
- 修复 `spawn forcesp` 没有生成失败提示的bug
- 现在 `spawn forcesp` 可以指定坐标
- 移除了 `prof entities` 的结果中的`minecraft:`前缀
