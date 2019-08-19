

# Bitdisk SDK v1.0 设计

## 需求功能
- [ ] HttpAPI登陆接口
- [ ] Websocket信令接口(Bson\chainsql)
- [ ] Webrtc数据传输通道
- [ ] ChunkManager文件数据存储
- [ ] DB适配器中间件
  - [ ] 统一使用LevelDB做应用存储
  - [ ] 各平台拥有自己的实现DB，CM反向调用
  - [ ] 适配器中间件，使用多种DB
- [x] ChunkCodec编解码

## 核心业务流程
- 用户上传文件：接收并存储上传数据，Token验证和确认
- 扩散自检：接受并完成自扩散任务（包括下载、解码）
- 响应广播：响应数据检查广播消息

## DB适配中间件

- CouchDB（android）
- objectbox（pcminer）
- mysql（linux）
- ios
- 
