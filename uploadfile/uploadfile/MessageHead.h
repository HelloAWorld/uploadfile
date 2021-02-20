#pragma once

//消息头定义
class CMessageHead
{
public:
    CMessageHead();
    ~CMessageHead();

    int GetMsgLength() const;
    int GetDataLen() const;
    const char* GetData() const;
    void Encode(int* iOutDataLen, char** pOutData);
    int GetEncodeDataLenght() const;

    void SetMsgLength(int iLength);
    void SetData(int iDataLen, const char* pData);

    int Decode(int iDataLen, const char* pData);
private:
    int m_msglength;   //消息长度
    int m_datalen;     //消息中数据长度
    char* m_pData;     //消息中实际数据
};