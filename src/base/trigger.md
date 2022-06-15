触发器的内建变量

物品

- {iname} 名称
- {iid} ID
- {iaux} 特殊值

方块

- {bname} 名称
- {bid} 方块ID
- {baux} 方块特殊值
- {bx}{by}{bz} 坐标

fmt::format("setblock {bx} {by} {bz} diamond",x,y,z)