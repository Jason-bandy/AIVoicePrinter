# BkFlashPartition.h 分区配置修改指南

## 问题场景
修改固件后遇到 `firmware size larger than app size` 错误。

## 解决方案

### 1. 修改分区配置
编辑 `beken378/func/user_driver/BkFlashPartition.h`，找到 2M 分区配置（约第 86-103 行）：

```c
[BK_PARTITION_APPLICATION] = {
    .partition_length = 0x134000,  // 增加此值以扩大 APP 分区
},
[BK_PARTITION_OTA] = {
    .partition_start_addr = 0x145000,  // 必须 >= app_start + app_length
}
```

### 2. CRC 换算公式
```
partition.json len = (BkFlashPartition.h partition_length / 34) * 32
```

### 3. 重新生成配置
**Windows**:
```powershell
.\tools\scripts\generate_sys_config.bat bk7252n
scons
```

**Mac/Linux**:
```bash
scons  # 自动调用生成脚本
```

### 4. 注意事项
- OTA 起始地址必须大于 APP 结束地址（4K 对齐）
- OTA 分区长度 / APP 分区长度 ≈ 0.6 以上（压缩比率要求）
- 修改后如果 `partition.json` 未更新，删除缓存文件重新生成

## 相关脚本
- `tools/scripts/generate_sys_config.bat` - 生成 rtconfig.h 和 partition.json
- `tools/beken_packager/gen_partition.py` - 解析 flash_partition.o 生成分区 JSON
- `tools/scripts/post_action.py` - 检查固件大小是否超出分区限制
