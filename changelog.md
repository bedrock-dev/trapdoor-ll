### trapdoor-ll-0.40.0-1.20.11.01

#### 更新日志

- 支持1.20.11.01
- 修复仙人掌对部分红石方块无效果的问题 (#111)


### trapdoor-ll-0.39.0-1.20.0.01

#### 更新日志

- 支持1.20.0.01
- 修复`log pt`命令崩溃的问题 (#103)
- 修复猫,灾厄村民等生物头顶出现时钟tag的问题 (#102)
- 修复假人背包无法保存的问题


### trapdoor-ll-0.38.0-1.19.81.01

#### 更新日志

- 支持1.19.81.01
- 修复低耐久不切换同样不同耐久的工具的情况
- 修改快捷指令的配置文件格式，为每个快捷指令添加名字以及`enable`选项。**因此该版本的配置文件的快捷指令部分不兼容旧版本，请手动替换快捷指令部分的配置**
- 修复仙人掌转方块和仙人掌右键混凝土的快捷指令冲突的问题
- 添加`shortcut`命令用于在游戏内开关快捷指令 (#96)

---

- Support 1.19.81.01
- Fix low damage tools can't switch automatically
- Modify the configuration file format of shortcuts, add name as key and `enable` option for each shortcut. Therefore,  **the 
  configuration file of this version is not compatible with the old version, please replace the configuration of
  the shortcut command manually**.
- Fix the issue that the cactus rotation are in conflict with shortcut  "hopper counter printer".
- Add `shortcut` command for en/disable shortcut in game (#96)

### trapdoor-ll-0.37.0-1.19.81.01

#### 更新日志

- 支持1.19.81.01
- 修复低耐久不切换同样不同耐久的工具的情况

---

- Support 1.19.81.01
- Fix Low damage tools can't switch automatically

### trapdoor-ll-0.36.0-1.19.72.01

#### 更新日志

- 支持1.19.72.01
- 为`prof`添加`Trapdoor`子项以测量插件自身的tick开销
- 优化配置文件的加载方式（便于多人开发）
- 为[hud]添加了显示全局生物上限的功能
- 修复[hud]的漏斗计数器无法正常关闭的bug

---

- Support 1.19.72.01
- Add `Trapdoor` items to the outputs of `prof` command to measure the tick overhead of mod itself.
- Add global spawn limit to hud command
- Fix a bug in [hud] command that the counter in hud can not be closed

### trapdoor-ll-0.35.0-1.19.63.01

#### 更新日志

- 修复假人部分刷物品的bug
- 调整假人的相关命令(`useonpos`,`useon`,`destroypos`,`destroy`)
- 为假人增加脚本执行功能，具体见[假人脚本API & 教程](https://bedrock-dev.github.io/tr-wiki/scripts.html)

---

- Fixed the bug of simulated player can duplicate items
- Adjust some `player` subcommands (`useonpos`,`useon`,`destroypos`,`destroy`)
- Add script execution features for simulated player,
  see [Dummy Script API & Tutorial](https://bedrock-dev.github.io/tr-wiki/scripts.html) for details
  (Due to limited time, the English document is suspended)

### trapdoor-ll-0.30.0-1.19.61.01

**由于`installer.exe`会自动判别LL版本，因此此后不再标出LL版本信息**

#### 更新日志

- 支持1.19.61.01
- 合并`func`和`tweak`
  指令的内的功能并移除`tweak`
  功能，且将其分为全局功能和个人功能，细节参考[这里](https://github.com/bedrock-dev/trapdoor-ll/blob/1.19.61.01/docs/func.md)
- 修改配置文件格式，且**不和之前的版本兼容**，请手动替换和编辑
- 添加`player <name> swap`功能，可交换玩家和其面向的假人的背包
- 更完善的多语言支持（包括中文和英文）
- 修复`player set`会修改假人背包内物品的bug
- 现在`hud hopper`不需要指向混凝土也能使用（自动显示玩家上一次交互的频道的信息）

---


**Due to `installer.exe` will automatically identify the LL version, so the changelog won't display LL version
information**

#### update log

- support 1.9.61.01
- Merge the features in the `func` and `tweak` command, remove `tweak command`, and divide them into global features
  and individual functions. For details, refer
  to [here](https://github.com/bedrock-dev/trapdoor-ll/blob/1.19.61.01/docs/func.md)
- Modify the configuration file format, and **not compatible with the previous version**, please replace and edit
  manually
- Add `player <name> swap` function, which can swap the backpack of the player and the fakeplayer
- More perfect I18N support (including Chinese and English)
- Fixed the bug that `player set` would modify the items in the fake player's backpack
- Now `hud hopper` can be used without pointing to concrete (automatically displays the information of the channel the
  player interacted with last time)

### trapdoor-ll-0.26.0-1.19.61.01

#### 依赖和版本支持

- 需要LiteLoader 2.10.0(-beta) 或以上版本
- 支持1.19.61.01

#### 更新日志

- 支持1.19.61.01

----

#### Dependencies and Version Support

- Requires LiteLoader 2.10.0(-beta) or above
- Supports 1.19.61.01

#### Changelog

- Supports 1.19.61.01

### trapdoor-ll-0.25.0-1.19.50.02

#### 依赖和版本支持

- 需要LiteLoader 2.9.0 或以上版本
- 支持1.19.50.x以及1.19.51.x及以上版本

#### 更新日志

- 移除`hud show true`命令，使用`hud add`后会自动显示
- 添加`player [name] set slot`子命令用于将背包内指定位置的物品交换到手上
- 添加`tweak creativenoclip`开启后创造模式能够穿墙
- 添加多语言支持(中文和英文)
- 优化`HSA`算法，现在不开生物生成，在虚空下也能显示HSA
- 在release中添加`installer.exe`简化安装流程

----

### trapdoor-ll-0.22.0-1.19.50.02

#### 依赖和版本支持

- 需要LiteLoader 2.9.0 或以上版本
- 支持1.19.50.x以及1.19.51.x及以上版本

#### 更新日志

- `hud redstone`现在会显示容器能提供的比较器信号
- 添加`prof pt`子命令用于评估区块内计刻数量
- 添加 `log pt`子命令用于显示区块内所有计划刻的详细信息
- 修复假人会攻击自己的问题
- 移除`tick`命令的部分倍率限制以及不能在命令方块内执行的限制
- 添加 `player info`命令用于显示假人的部分元数据信息
- 修复召唤假人时其游戏模式可能和召唤者不一致的问题
- 修复`village head`无法关闭村民头顶信息显示的问题
- 调高部分命令的权限等级
- 修复一些typo

----

#### Dependencies and Version Support

- Requires LiteLoader 2.9.0 or above
- Supports 1.19.50.x and 1.19.51.x

#### Changelog

- `hud redstone` now displays the comparator signals that container can provide
- Added the `prof pt` subcommand to evaluate the number of pending ticks in a chunk
- Added `log pt` subcommand to display detailed information about all the pending ticks in a chunk
- Fix the problem that fake player can attack themselves
- Removed some restrictions of the `tick` command and the restriction that it cannot be executed
  within a command block
- Add `player info` command to show some metadata information of the fake player
- Fix the problem that the game mode of the fake player may not match with command executor
- Fix the problem that `village head` cannot turn off the display of villagers' head information.
- Increase the default permission level of some commands.
- Fix some typos

### trapdoor-ll-0.21.0-1.19.50.02

#### **注意**

此版本需要同时安装

- [https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0](https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0)
- [https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0](https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0)

这两个材质包，如果不使用区块边界和史莱姆区块显示功能，可不安装第二个

#### 依赖和版本支持

- 需要LiteLoader 2.9.0 或以上版本
- 支持1.19.50.x以及1.19.51.x及以上版本

#### 更新日志

- 支持1.19.5x

----

Both the follow 2 resource packs are required

- [https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0](https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0)
- [https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0](https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0)

#### Dependencies and Version Support

- Requires LiteLoader 2.9.0 or above
- Supports 1.19.50.x and 1.19.51.x

#### Changelog

- Support 1.19.5x

### trapdoor-ll-0.20.0-1.19.40.02

#### **注意**

此版本需要同时安装

- [https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0](https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0)
- [https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0](https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0)

这两个材质包，如果不使用区块边界和史莱姆区块显示功能，可不安装第二个

#### 依赖和版本支持

- 需要LiteLoader 2.8.0 或以上版本
- 理论上支持1.19.40.02及以上版本

#### 更新日志

- 支持1.19.40.02

----

Both the follow 2 resource packs are required

- [https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0](https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0)
- [https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0](https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0)

#### Dependencies and Version Support

- Requires LiteLoader 2.8.0 or above
- Theoretically supports 1.19.40.02 and above

#### Changelog

- Support 1.19.40.02

### trapdoor-ll-0.19.5-1.19.30.04

#### **注意**

此版本需要同时安装

- [https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0](https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0)
- [https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0](https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0)

这两个材质包，如果不使用区块边界和史莱姆区块显示功能，可不安装第二个

#### 依赖和版本支持

- 需要LiteLoader 2.7.2 或以上版本
- 理论上支持1.19.30.04及以上版本

#### 更新日志

- 支持1.19.30.04
- 新增`village dweller`命令以显示村民所绑定的POI坐标
- `HUD`新增`villager`选项，功能同上
- `data entity`会显示更加精确的坐标和速度数据(#37)
- 现在内建命令`ll list Trapdoor`会显示正确的版本信息了
- 修复有关漏斗计数器的一系列bug(#38)

### trapdoor-ll-0.19.1-1.19.22.02

#### **注意**

此版本需要同时安装

- [https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0](https://github.com/OEOTYAN/LiteLoaderBDS-CUI/releases/tag/ll.1.0)
- [https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0](https://github.com/OEOTYAN/Trapdoor-CUI/releases/tag/v5.0)

这两个材质包，如果不使用区块边界和史莱姆区块显示功能，可不安装第二个

#### 依赖和版本支持

- 需要LiteLoader 2.6.1 或以上版本
- 理论上支持1.19.22.01及以上版本

#### 更新日志

- 将粒子调整为LL统一使用的材质包
- 在默认配置文件中移除了假人的默认前缀`bot_`
- `village info`命令支持补全(同时支持vid和uuid)
- 假人支持自动寻路到某处(`player <name> navto <pos>`)
- 假人支持跟随(除了掉落物外的任意)实体，使用指针指定实体，不指向任何实体时假人会跟随自己(`player <name> follow`)
- 假人支持丢出背包内物品
    - `drop`丢出某种物品一组
    - `droptype`丢出背包内所有该种物品
    - `dropall`丢出背包内所有物品(不包含盔甲栏)
- 修复异步保存假人背包导致的潜在刷物品的bug
- 假人现在可以执行命令了(`player <name> runcmd <command> ...`)
- 修复部分typo错误

### trapdoor-ll-0.18.0-1.19.22.02

#### 依赖和版本支持

- 需要LiteLoader 2.6.1 或以上版本
- 理论上支持1.19.22.01及以上版本

#### 更新日志

- 支持1.19.22.01,(应该也支持1.19.21.01)

### trapdoor-ll-0.18.0-1.19.20.02

#### 依赖和版本支持

- 需要LiteLoader 2.5.0 或以上版本
- 理论上支持1.19.20.02及以上版本

#### 更新日志

- 开服时的默认配置项将不再打印在控制台
- 添加`traodoor dump`命令以打印当前配置项
- `tweak`添加防止爆炸破坏地形的功能
- 修复假人背包数据同步滞后的问题（#20）
- 修复假人在死亡时背包不能正确清空的问题 (#24)
- 添加自定义假人名称前缀的功能（生成新假人时会使用）(#21)
- 移除假人自动加入游戏的功能 (#23)
- 修复`tick fz`和`tick fw`会执行两次的问题
- 修复仙人掌转方块和漏斗计数器查看冲突(#28)
- 修复部分拼写错误

### trapdoor-ll-0.17.5-1.19.20.02

#### 依赖和版本支持

- 需要LiteLoader 2.5.0 或以上版本
- 理论上支持1.19.20.02及以上版本

#### 更新日志

- `tweak`命令现在可以修改计划刻上限了
- 修复`prof`指令内计划刻数据计算错误的问题
- 修复无法将假人背包数据写如文件的问题(#12)
- 为`prof entity`增加合计执行时间
- 添加trigger命令以监控部分游戏内事件(新增事件可前往#17反馈)
- 移除占地方的banner并添加相关版权信息和构建时间
- 修复村民头顶信息的部分显示问题
- 修复玩家实体名字显示为`entity.player.name`的问题``

### trapdoor-ll-0.17-1.19.20.02

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
