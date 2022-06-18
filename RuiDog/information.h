#ifndef INFORMATION_H
#define INFORMATION_H
#include <string>
#include <vector>
using namespace std;

typedef enum{
    TYPE_GROUP_CHAT,TYPE_PRIVATE_CHAT
}EChatType;

struct UserInfo{//当前用户登录信息
    int id = 0;
    string name;
    char pwd[50] = {0};
};

struct FriendInfo{
    string name;//好友用户名
    int id;//好友账号
    string state;//好友状态
};

struct GroupUser{//群组成员信息
    int id;
    string name;
    string role;
};

struct GroupInfo
{
    string m_groupName; //群名称
    int  m_account;       //群账号
    string m_groupDesc; //群描述
    vector<GroupUser> users;//群用户
};

struct AddFriendInfo
{
    int friendId;    //好友账号
    int myId;    //发送端账号
    char requestInfo[64];    //请求信息 例如我是xxx
};

struct AddGroupInfo
{
    int groupId;    //群组id
    int myId;   //发送段账号
    char requestInfo[64];   //请求信息  例如我是xxx
};

struct CreateGroupInfo
{
    string m_groupName;   //群组id
    string m_groupDesc; //群描述
};

struct ImgInfo
{
    int friendId;//好友账号
    string friendName;//好友昵称
    string imgPath;//图片路径
    int groupId;//群组id
    int chatType;//聊天类型，分为群聊和单聊
};

#endif // INFORMATION_H
