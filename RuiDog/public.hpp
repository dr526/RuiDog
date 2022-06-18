#ifndef PUBLIC_H
#define PUBLIC_H

/*
server和client的公共文件
*/
enum EnMsgType
{
    LOGIN_MSG = 1,    //登陆消息
    LOGIN_MSG_ACK,    //登录响应消息
    REG_MSG,          //注册消息
    REG_MSG_ACK,      //注册响应消息
    ONE_CHAT_MSG,     //聊天消息
    ADD_FRIEND_MSG,   //添加好友消息
    ADD_FRIEND_ACK,  //添加好友响应消息
    ADD_FRIEND_REQUEST,//发送添加好友请求
    ADD_FRIEND_RESPONSE,//添加好友的响应
    CREATE_GROUP_MSG, //创建群组
    CREATE_GROUP_ACK, //创建群组响应
    ADD_GROUP_MSG,    //加入群组聊天
    ADD_GROUP_REQUEST, //发送添加群组请求
    ADD_GROUP_RESPONSE, //添加群组的响应
    ADD_GROUP_ACK,     //添加群组响应消息
    GROUP_CHAT_MSG,   //群聊天
    LOGINOUT_MSG,      //退出登陆
    IMG_MSG,           //图片信息
    FILE_MSG,           //文件信息
    Query_FRIEND_MSG,    //查询好友信息
    Query_FRIEND_ACK,    //查询好友响应消息
    Query_GROUP_MSG,    //查询群组信息
    Query_GROUP_ACK,     //查询群组响应信息
    ONE_VOICE_CHAT,     //单聊语音聊天
    GROUP_VOICE_CHAT,    //群聊语音聊天
    UPDATE_USER_INFO,   //修改用户密码
    UPDATE_USER_ACK     //修改用户密码成功
};
#endif
