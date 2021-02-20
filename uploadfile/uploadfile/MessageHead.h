#pragma once

//��Ϣͷ����
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
    int m_msglength;   //��Ϣ����
    int m_datalen;     //��Ϣ�����ݳ���
    char* m_pData;     //��Ϣ��ʵ������
};