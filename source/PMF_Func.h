//////////////////////////////////////////////////////////////////////
// ��Ȩ (C), 1988-1999, XXXX��˾
// �� �� ��: PMF_Func.h
// ��    ��:       �汾:        ʱ��: // ���ߡ��汾���������
// ��    ��: ��ά�������������ռ�ӿں�������
// ��    ������ #include <filename.h> ��ʽ�����ñ�׼���ͷ�ļ������������ӱ�׼��Ŀ¼��ʼ������
//           �� #include "filename.h" ��ʽ�����÷Ǳ�׼���ͷ�ļ��������������û��Ĺ���Ŀ¼��ʼ������
// �޸ļ�¼:     // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                 // �߼��޸����ݼ���  
// 1. ʱ��:
//    ����:
//    �޸�����:
// 2. ...
//////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _PMF_FUNC_H__
#define _PMF_FUNC_H__

#include "LE_SymSprsMatDef.h"
//-----------��������--------------------------------------------

// EMT���������
extern void main_sim();
// ָ���������ֵ
extern void initMem();
// ���������ļ�
extern void readInNet(const char *pcProjIn);
// ����������������ýӵر��
extern void setGndTagNet();
// �������
extern void checkParaNet();
// ���򳣹�������ʽ��ʼ��
extern void initNet_Standard(char *sFile_In,char *sProject_Out);
// ��ż���
extern void acdcemtcal_ndmeth(const char *pcProjIn,const char *pcProjOut);
// ���ݴ洢
extern void transformNet();
// ָ������ڴ��ͷ�
extern void deallocateNet();
// ������ֵ���㷽�����΢�ַ���
extern void stepNet(const char *pcProjOut);
// Ԫ����ϵͳע�����Դ���и���
extern void stepI0Net(VecRealStru *pI0);

#endif