//////////////////////////////////////////////////////////////////////
// 版权 (C), 1988-1999, XXXX公司
// 文 件 名: PMF_Func.h
// 作    者:       版本:        时间: // 作者、版本及完成日期
// 描    述: 二维及以上数组分配空间接口函数声明
// 其    他：用 #include <filename.h> 格式来引用标准库的头文件（编译器将从标准库目录开始搜索）
//           用 #include "filename.h" 格式来引用非标准库的头文件（编译器将从用户的工作目录开始搜索）
// 修改记录:     // 修改历史记录列表，每条修改记录应包括修改日期、修改
                 // 者及修改内容简述  
// 1. 时间:
//    作者:
//    修改内容:
// 2. ...
//////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _PMF_FUNC_H__
#define _PMF_FUNC_H__

#include "LE_SymSprsMatDef.h"
//-----------函数声明--------------------------------------------

// EMT主程序入口
extern void main_sim();
// 指针变量赋空值
extern void initMem();
// 读入数据文件
extern void readInNet(const char *pcProjIn);
// 根据输入参数，设置接地标记
extern void setGndTagNet();
// 参数检查
extern void checkParaNet();
// 程序常规启动方式初始化
extern void initNet_Standard(char *sFile_In,char *sProject_Out);
// 电磁计算
extern void acdcemtcal_ndmeth(const char *pcProjIn,const char *pcProjOut);
// 数据存储
extern void transformNet();
// 指针变量内存释放
extern void deallocateNet();
// 采用数值计算方法求解微分方程
extern void stepNet(const char *pcProjOut);
// 元件对系统注入电流源进行更新
extern void stepI0Net(VecRealStru *pI0);

#endif