#include "MessageHead.h"
#include "memory.h"


CMessageHead::CMessageHead()
{
    m_msglength = 0;
    m_datalen = 0;
    m_pData = nullptr;
}

CMessageHead::~CMessageHead()
{
    if (nullptr != m_pData)
    {
        delete [] m_pData;
    }
}

int CMessageHead::GetMsgLength() const
{
    return m_msglength;
}

int CMessageHead::GetDataLen() const
{
    return m_datalen;
}

const char* CMessageHead::GetData() const
{
    return m_pData;
}

void CMessageHead::Encode(int* iOutDataLen, char** pOutData)
{
    int len = 0;
    int iEncodedLen = 0;
    *iOutDataLen = GetEncodeDataLenght();
    char* pTmpData = new char[*iOutDataLen];

    len = sizeof(m_msglength);
    memcpy(pTmpData + iEncodedLen, &m_msglength, len);
    iEncodedLen += len;

    len = sizeof(m_datalen);
    memcpy(pTmpData + iEncodedLen, &m_datalen, len);
    iEncodedLen += len;

    memcpy(pTmpData + iEncodedLen, m_pData, m_datalen);
    iEncodedLen += m_datalen;

    *pOutData = pTmpData;
}

int CMessageHead::GetEncodeDataLenght() const
{
    int iEncodeLen = 0;
    iEncodeLen += sizeof(m_msglength);
    iEncodeLen += sizeof(m_datalen);
    iEncodeLen += m_datalen;
    return iEncodeLen;
}

void CMessageHead::SetMsgLength(int iLength)
{
    m_msglength = iLength;
}

void CMessageHead::SetData(int iDataLen, const char* pData)
{
    m_datalen = iDataLen;
    if (m_pData != nullptr)
    {
        delete[] m_pData;
        m_pData = nullptr;
    }
    m_pData = new char[m_datalen];
    memcpy(m_pData, pData, m_datalen);
}

int CMessageHead::Decode(int iDataLen, const char* pData)
{
    int len = 0;
    int iDecodedLen = 0;

    len = sizeof(m_msglength);
    if (iDataLen < iDecodedLen + len)
    {
        return -1;
    }
    memcpy(&m_msglength, pData + iDecodedLen, len);
    iDecodedLen += len;

    len = sizeof(m_datalen);
    if (iDataLen < iDecodedLen + len)
    {
        return -1;
    }
    memcpy(&m_datalen, pData + iDecodedLen, len);
    iDecodedLen += len;

    if (nullptr != m_pData)
    {
        delete[] m_pData;
    }
    len = m_datalen;
    if (iDataLen < iDecodedLen + len)
    {
        return -1;
    }
    m_pData = new char[len];
    memcpy(m_pData, pData + iDecodedLen, len);
    iDecodedLen += len;

    return iDecodedLen;
}
