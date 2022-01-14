#pragma once
#include<bits/stdc++.h>
#include<windows.h>
#include<csignal>
#include<pthread.h>
#include<vector>
#include<queue>
#include<stack>
#include<bitset>
#include<string>
#include <mmsystem.h>
#include <iostream>
#include<fstream>
#include<math.h>
#include <vector>
#define WAVE_HEAD_LENGTH 44//wav头文件长度
#define m_samplefreq 22050 //每秒采样数-采样速率(22050Hz)
#define m_channels 1 //通道数量，1为单通道，2为双通道
#define m_channelbits 8
#define MATH_PI 3.1415
#define C4 523
#define c4 554
#define D4 588
#define d4 623
#define E4 660
#define F4 700
#define f4 742
#define G4 786
#define g4 833
#define A4 884
#define a4 936
#define B4 992
#define C5 1046
#define c5 1108
#define D5 1176
#define d5 1246
#define E5 1320
#define F5 1400
#define f5 1484
#define G5 1572
#define g5 1666
#define A5 1768
#define a5 1872
#define B5 1984
#define C6 2092
#define c6 2216
#define D6 2352
#define d6 2492
#define E6 2640
#define F6 2800
#define f6 2968
#define G6 3144
#define g6 3332
#define A6 3536
#define a6 3744
#define B6 3968
using namespace std;
map<float,int> sound_dict;
void init_dict()
{
	sound_dict[1]=C4;
	sound_dict[1.5]=c4;
	sound_dict[2]=D4;
	sound_dict[2.5]=d4;
	sound_dict[3]=E4;
	sound_dict[3.5]=F4;
	sound_dict[4]=F4;
	sound_dict[4.5]=f4;
	sound_dict[5]=G4;
	sound_dict[5.5]=g4;
	sound_dict[6]=A4;
	sound_dict[6.5]=a4;
	sound_dict[7]=B4;
	sound_dict[7.5]=C5;
	sound_dict[11]=C5;
	sound_dict[11.5]=c5;
	sound_dict[12]=D5;
	sound_dict[12.5]=d5;
	sound_dict[13]=E5;
	sound_dict[13.5]=F5;
	sound_dict[14]=F5;
	sound_dict[14.5]=f5;
	sound_dict[15]=G5;
	sound_dict[15.5]=g5;
	sound_dict[16]=A5;
	sound_dict[16.5]=a5;
	sound_dict[17]=B5;
	sound_dict[17.5]=C6;
	sound_dict[21]=C6;
	sound_dict[21.5]=c6;
	sound_dict[22]=D6;
	sound_dict[22.5]=d6;
	sound_dict[23]=E6;
	sound_dict[23.5]=F6;
	sound_dict[24]=F6;
	sound_dict[24.5]=f6;
	sound_dict[25]=G6;
	sound_dict[25.5]=g6;
	sound_dict[26]=A6;
	sound_dict[26.5]=a6;
	sound_dict[27]=B6;
}
inline int conv(float id)
{
	return sound_dict[id];
}
//.wav文件的文件头结构
typedef struct
{
	char chRIFF[4]; //头部那个RIFF;
	DWORD dwRIFFLen; //存的是后面所有文件的大小;
	char chWAVE[4];
	char chFMT[4];
	DWORD dwFMTLen; //存的是fmt保存的大小，包含这之后，data前面几个，共16个;
	PCMWAVEFORMAT pwf;
	char chDATA[4];
	DWORD dwDATALen; //数据的长度;
//UINT8* pBufer;
} WaveHeader;
//采样率、频率、音量、数据空间、采样点数
void MakeWaveData(int rate, int freq, int amp, char* p, int len, double synth(double)=sin)
{
	int flag = 0;
	if (m_channelbits == 16) //16位
	{
		if (m_channels == 1)
		{
			for (int i = 0; i < len; i++)
			{
				INT16 v = INT16(amp/100*32768 * synth(2 * MATH_PI * freq * i / rate));
				*(p + flag) = v & 0xFF;//低8位
				*(p + flag + 1) = (v >> 8) & 0xFF;//16bit量化 高8位
				flag += 2;
			}
		}
		else
		{
			for (int i = 0; i < len; i++)
			{
				INT16 vl = INT16(amp / 100 * 32768 * synth(2 * MATH_PI * freq * i / rate)) ;
				INT16 vr = INT16(amp / 100 * 32768 * synth((2 * MATH_PI * freq * (i+5) )/ rate)) ;
				*(p + flag) = (vl & 0xFF);
				*(p + flag + 1) = ((vl >> 8) & 0xFF);
				*(p + flag + 2) = (vr & 0xFF);
				*(p + flag + 3) = ((vr >> 8) & 0xFF);
				flag += 4;
			}
		}
	}
	else
	{
		if (m_channels == 1)
		{
			for (int i = 0; i < len; i++)
			{
				*(p + i) = char(synth(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100 + 128);
			}
		}
		else
		{
			for (int i = 0; i < len; i++)
			{
				*(p + flag)= char(synth(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100+128);
				*(p + flag + 1)= char(synth((i+5) * (MATH_PI * 2) / rate * freq) * amp * 128 / 100+128);
				flag += 2;
			}
		}
	}
}
//频率、音量、持续时间、wav文件保存路径(包含文件名+后缀
int create_wave(vector<int> freqVec, int volume, float durations, string wavPath, double synth(double)=sin)
{
	WaveHeader *pHeader = new WaveHeader;
	DWORD totalLen = (m_samplefreq * m_channels * m_channelbits / 8) * durations + 44;//文件总长度=(采样率 * 通道数 * 比特数 / 8) * 持续时间(s)
	pHeader->chRIFF[0] = 'R';
	pHeader->chRIFF[1] = 'I';
	pHeader->chRIFF[2] = 'F';
	pHeader->chRIFF[3] = 'F';
	pHeader->dwRIFFLen = totalLen - 8;//文件的总长度-8bits
	pHeader->chWAVE[0] = 'W';
	pHeader->chWAVE[1] = 'A';
	pHeader->chWAVE[2] = 'V';
	pHeader->chWAVE[3] = 'E';
	pHeader->chFMT[0] = 'f';
	pHeader->chFMT[1] = 'm';
	pHeader->chFMT[2] = 't';
	pHeader->chFMT[3] = ' ';
	pHeader->dwFMTLen = 0x0010;//一般情况下Size为16，如果为18则最后多了2个字节的附加信息
	pHeader->pwf.wf.wFormatTag = 0x0001;//波形声音的格式-编码方式
	pHeader->pwf.wf.nChannels = m_channels; //通道数量，1为单通道，2为双通道
	pHeader->pwf.wf.nSamplesPerSec = m_samplefreq; //采样速率 =44.1KHz
	pHeader->pwf.wf.nAvgBytesPerSec = m_samplefreq * m_channels * m_channelbits / 8;//平均数据传输率，每秒所需字节数、缓冲区大小
	pHeader->pwf.wf.nBlockAlign = m_channels * m_channelbits / 8;//以字节为单位设置块对齐，一个采样的字节数
	pHeader->pwf.wBitsPerSample = m_channelbits;//每次采样样本的大小；16位，即设置PCM的方式为16位立体声(双通道)
	pHeader->chDATA[0] = 'd';
	pHeader->chDATA[1] = 'a';
	pHeader->chDATA[2] = 't';
	pHeader->chDATA[3] = 'a';
	pHeader->dwDATALen = totalLen - WAVE_HEAD_LENGTH;//数据的长度，=文件总长度-头长度(44bit)
	char *pWaveBuffer = new char[totalLen]; //音频数据
	memcpy(pWaveBuffer, pHeader, WAVE_HEAD_LENGTH);
	int freNum = freqVec.size();
	int singleLen = m_samplefreq*durations/freNum;
	char* tempBuffer = pWaveBuffer + WAVE_HEAD_LENGTH;
	for ( int iFre = 0; iFre < freNum; ++iFre )
	{
		if ( iFre > 0 )
		{
			tempBuffer += singleLen;
		}
//TODO:最后一个参数
		MakeWaveData(pHeader->pwf.wf.nSamplesPerSec, freqVec[iFre], volume, tempBuffer, singleLen, synth);//采样点数
	}
	ofstream fout;
	fout.open(wavPath, ios::out | ios::binary);//以二进制形式打开文件
	if (fout)
		fout.write(pWaveBuffer, totalLen);
	else
		return 0;
	fout.close();
	delete(pHeader);
	return 1;
}
vector<int> v;
void sound(float id,int time)
{
	v.push_back(conv(id));
}
int main(int argc,char** argv)
{
	init_dict();
	string fn;
	if(argc!=2)
	{
		cout<<"filename:";
		cin>>fn;
	}
	else
		fn=argv[1];
	ifstream fin(fn.c_str());
	int speed,time;
	cout<<"reading settings(speed&note length)...";
	fin>>speed>>time;
	cout<<"done";
	float note;
	cout<<"\nreading notes...";
	while(fin>>note)
		sound(note,time);
	cout<<"done";
	cout<<"\ngenerating WAV audio...";
	create_wave(v,100,4500.0/speed,fn.substr(0,fn.length()-4)+"_tan.wav",tan);
	cout<<"done";
}
