### Shortcuts教程

shortcut可以做到游戏内玩家触发某种行为后执行特定的指令序列，目前支持以下三种：

### use，当玩家右键物品时触发

```json
 {
      "type": "use", //触发器类型，固定
      "item": "cactus", //使用的物品，支持xx:1的样式来指定特殊值
      "prevent": false, //是否阻止原有的事件发生
      "actions": [ //执行的指令系序列
        "counter print"
      ]
    }
```

可用的内建变量

- iname 物品名字
- iaux 物品特殊值
- px 玩家x坐标
- py 玩家y坐标
- pz 玩家z坐标

### use-on 当玩家用某种物品右键某种方块时触发

```json
 "shortcuts": [
    {
      "type": "use-on", //触发器类型，固定
      "item": "cactus", //使用的物品，支持name:aux的样式来指定特殊值
      "block": "concrete", //被右键的方块，支持name:aux的样式来指定特殊值
      "prevent": false, //是否阻止原有的事件发生
      "actions": [ //执行的指令系序列
        "counter print {baux}"
      ]
    },
```

可用的内建变量

- iname 物品名字
- iaux 物品特殊值
- bname 右键的方块名字
- baux 右键的方块特殊值
- bx 方块x坐标
- by 方块y坐标
- bz 方块 z坐标
- px 玩家x坐标
- py 玩家y坐标
- pz 玩家z坐标

### 自定义命令

```json
{
  "type": "command", //触发器类型，固定
  "command": "here", //快捷命令名称
  "actions": [ //要执行的指令序列
    "/say Position: [{px} , {py} , {pz}]"
  ]
}
```

可用的内建变量

- px 玩家x坐标
- py 玩家y坐标
- pz 玩家z坐标
