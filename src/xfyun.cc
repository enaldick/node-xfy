/*
* 语音识别（Automatic Speech Recognition）技术能够从语音中识别出特定的命令词或语句模式。
*/
#include <node.h>
#include <nan.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __linux
#include <unistd.h>
#endif

#include "qisr.h"
#include "qivw.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#define BUFFER_SIZE 4096
#define HINTS_SIZE 100
#define GRAMID_LEN 128
#define FRAME_LEN 640

namespace xfy
{

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Null;
using v8::Object;
using v8::String;
using v8::Value;

int g_awaken_ret = 0;
int cb_ivw_msg_proc( const char *sessionID, int msg, int param1, int param2, const void *info, void *userData )
{
	if (MSP_IVW_MSG_ERROR == msg) //唤醒出错消息
	{
		printf("\n\nMSP_IVW_MSG_ERROR errCode = %d\n\n", param1);
	}
	else if (MSP_IVW_MSG_WAKEUP == msg) //唤醒成功消息
	{
		printf("\n\nMSP_IVW_MSG_WAKEUP result = %s\n\n", info);
	}

	g_awaken_ret = msg;

	printf("\n\nMSP_IVW_MSG_WAKEUP nothing : %d-%d\n\n", g_awaken_ret,msg);


	return 0;
}

void awaken(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
	if (info.Length() != 6)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
	}
	String::Utf8Value username(info[0]->ToString());
	String::Utf8Value password(info[1]->ToString());
	String::Utf8Value login_params(info[2]->ToString());
	String::Utf8Value session_params(info[3]->ToString());
	String::Utf8Value audio_file(info[4]->ToString());

    char *rec_error = "";
    int         ret = MSP_SUCCESS;
	/* 用户登录 */
	ret = MSPLogin(NULL, NULL, (const char *)(*login_params)); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret)
	{
		printf("MSPLogin failed, error code: %d.\n", ret);
		Nan::ThrowError("MSPLogin failed. Check out error code in consolelog.");
	}
	else
	{
		printf("MSPLogin success.\n");
	}

    const char *session_id = NULL;
	int err_code = MSP_SUCCESS;
	FILE *f_aud = NULL;
	long audio_size = 0;
	long real_read = 0;
	long audio_count = 0;
	int count = 0;
	int audio_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	char *audio_buffer=NULL;
	char sse_hints[128];
	if (NULL == (const char *)(*audio_file))
	{
		printf("params error\n");
        Nan::ThrowError("params error audio filename not to be empty.");
	}

	f_aud = fopen((const char *)(*audio_file), "rb");
	if (NULL == f_aud)
	{
		printf("audio file open failed! \n\n");
		 Nan::ThrowError("open audio_file error.");
	}
	fseek(f_aud, 0, SEEK_END);
	audio_size = ftell(f_aud);
	fseek(f_aud, 0, SEEK_SET);
	audio_buffer = (char *)malloc(audio_size);
	if (NULL == audio_buffer)
	{
		printf("malloc failed! \n");
		rec_error = "malloc failed! \n";
	}
	real_read = fread((void *)audio_buffer, 1, audio_size, f_aud);
	if (real_read != audio_size)
	{
		printf("read audio file failed!\n");
		rec_error = "read audio file failed!\n";
	}

	session_id = QIVWSessionBegin(NULL, (const char *)(*session_params), &err_code);
	if (err_code != MSP_SUCCESS)
	{
		printf("QIVWSessionBegin failed! error code:%d\n",err_code);
		rec_error = "QIVWSessionBegin failed! error code.\n";
	}

	err_code = QIVWRegisterNotify(session_id, cb_ivw_msg_proc,NULL);
	if (err_code != MSP_SUCCESS)
	{
//		_snprintf(sse_hints, sizeof(sse_hints), "QIVWRegisterNotify errorCode=%d", err_code);
		printf("QIVWRegisterNotify failed! error code:%d\n",err_code);
		rec_error = "QIVWRegisterNotify failed! error code";
	}
	if(""==rec_error){
        while(1)
        {
            long len = 10*FRAME_LEN; //16k音频，10帧 （时长200ms）
            audio_stat = MSP_AUDIO_SAMPLE_CONTINUE;
            if(audio_size <= len)
            {
                len = audio_size;
                audio_stat = MSP_AUDIO_SAMPLE_LAST; //最后一块
            }
            if (0 == audio_count)
            {
                audio_stat = MSP_AUDIO_SAMPLE_FIRST;
            }

            printf("csid=%s,count=%d,aus=%d\n",session_id, count++, audio_stat);
            err_code = QIVWAudioWrite(session_id, (const void *)&audio_buffer[audio_count], len, audio_stat);

            if (MSP_SUCCESS != err_code)
            {
                printf("QIVWAudioWrite failed! error code:%d\n",err_code);
    //			_snprintf(sse_hints, sizeof(sse_hints), "QIVWAudioWrite errorCode=%d", err_code);
                rec_error = "QIVWAudioWrite failed!";
            }
            if (MSP_AUDIO_SAMPLE_LAST == audio_stat)
            {
                break;
            }
            audio_count += len;
            audio_size -= len;

    //		sleep_ms(200); //模拟人说话时间间隙，10帧的音频时长为200ms
        }
    //	_snprintf(sse_hints, sizeof(sse_hints), "success");
        printf("QIVWAudioWrite success! \n");
	}else{
	    printf("(xfyun.cc)>>>>Can not start Session: %s", rec_error);
    }

	ret = MSPLogout();
    if (MSP_SUCCESS != ret)
    {
       printf("(xfyun.cc)>>>>MSPLogout failed, error code is: %d", ret);
    }

	if (NULL != session_id)
	{
		QIVWSessionEnd(session_id, sse_hints);
	}
	if (NULL != f_aud)
	{
		fclose(f_aud);
	}
	if (NULL != audio_buffer)
	{
		free(audio_buffer);
	}

   /**
   * return callback
   */
  Isolate *isolate = info.GetIsolate();
  // callback fn
  Local<Function> cb = Local<Function>::Cast(info[5]);
  const unsigned argc = 2;

  Local<Value> argv[argc] = { String::NewFromUtf8(isolate, rec_error), String::NewFromUtf8(isolate, g_awaken_ret == MSP_IVW_MSG_WAKEUP?"success":"faild") };
  cb->Call(Null(isolate), argc, argv);
}

void Iat(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
  printf("%s", info[0] -> ToString());
  if (info.Length() != 6)
  {
    Nan::ThrowTypeError("Wrong number of arguments");
  }
  String::Utf8Value username(info[0]->ToString());
  String::Utf8Value password(info[1]->ToString());
  String::Utf8Value login_params(info[2]->ToString());
  String::Utf8Value session_params(info[3]->ToString());
  String::Utf8Value audio_file(info[4]->ToString());

  // printlog
  printf("(xfyun.cc)>>>> username [%s].\n", (const char *)(*username));
  printf("(xfyun.cc)>>>> password [%s].\n", (const char *)(*password));
  printf("(xfyun.cc)>>>> login_params [%s].\n", (const char *)(*login_params));
  printf("(xfyun.cc)>>>> session_params [%s].\n", (const char *)(*session_params));
  printf("(xfyun.cc)>>>> audio_file [%s].\n", (const char *)(*audio_file));

  int ret = MSP_SUCCESS;
  char *grammar_id = NULL;

  /* 用户登录 */
  ret = MSPLogin(NULL, NULL, (const char *)(*login_params)); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数
  if (MSP_SUCCESS != ret)
  {
    printf("MSPLogin failed, error code: %d.\n", ret);
    Nan::ThrowError("MSPLogin failed. Check out error code in consolelog.");
  }
  else
  {
    printf("MSPLogin success.\n");
  }

  // char rec_result = run_iat((const char *)(*audio_file), (const char *)(*session_params));

  // process rec_result
  const char *session_id = NULL;
  char rec_result[BUFFER_SIZE] = {NULL};
  char *rec_error = "";
  char hints[HINTS_SIZE] = {NULL}; //hints为结束本次会话的原因描述，由用户自定义
  unsigned int total_len = 0;
  int aud_stat = MSP_AUDIO_SAMPLE_CONTINUE; //音频状态
  int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;  //端点检测
  int rec_stat = MSP_REC_STATUS_SUCCESS;    //识别状态
  int errcode = MSP_SUCCESS;

  FILE *f_pcm = NULL;
  char *p_pcm = NULL;
  long pcm_count = 0;
  long pcm_size = 0;
  long read_size = 0;

  bool testState = (NULL == (const char *)(*audio_file));
    printf("testState:%d.\n",testState?1:0);

  if (NULL == (const char *)(*audio_file))
    Nan::ThrowError("audio_file does not exist.");

  f_pcm = fopen((const char *)(*audio_file), "rb");
  if (NULL == f_pcm)
  {
    // printf("\nopen [%s] failed! \n", (const char *)(*audio_file));
    Nan::ThrowError("open audio_file error.");
  }

  fseek(f_pcm, 0, SEEK_END);
  pcm_size = ftell(f_pcm); //获取音频文件大小
  fseek(f_pcm, 0, SEEK_SET);

  p_pcm = (char *)malloc(pcm_size);
  if (NULL == p_pcm)
  {
    printf("\nout of memory! \n");
    rec_error = "filebuffer out of memory.";
  }

  read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //读取音频文件内容
  if (read_size != pcm_size)
  {
    printf("\nread [%s] error!\n", (const char *)(*audio_file));
    rec_error = "read audio file error.";
  }

  printf("(xfyun.cc)>>>>Start iat session ...\n");
  session_id = QISRSessionBegin(NULL, (const char *)(*session_params), &errcode); //听写不需要语法，第一个参数为NULL
  if (MSP_SUCCESS != errcode)
  {
    printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
    rec_error = "QISRSessionBegin failed! error code";
  }

  if ("" == rec_error)
  {
    while (1)
    {
      unsigned int len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
      int ret = 0;

      if (pcm_size < 2 * len)
        len = pcm_size;
      if (len <= 0)
        break;

      aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
      if (0 == pcm_count)
        aud_stat = MSP_AUDIO_SAMPLE_FIRST;

      ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
      if (MSP_SUCCESS != ret)
      {
        printf("\nQISRAudioWrite failed! error code:%d\n", ret);
        rec_error = "QISRAudioWrite failed! error code";
      }

      pcm_count += (long)len;
      pcm_size -= (long)len;

      if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
      {
        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
        if (MSP_SUCCESS != errcode)
        {
          printf("\nQISRGetResult failed! error code: %d\n", errcode);
          rec_error = "QISRGetResult failed! error code";
        }
        if (NULL != rslt)
        {
          unsigned int rslt_len = strlen(rslt);
          total_len += rslt_len;
          if (total_len >= BUFFER_SIZE)
          {
            printf("\nno enough buffer for rec_result !\n");
            rec_error = "no enough buffer for rec_result !";
          }
          strncat(rec_result, rslt, rslt_len);
        }
      }

      if (MSP_EP_AFTER_SPEECH == ep_stat)
        break;
      // usleep(200 * 1000); //模拟人说话时间间隙。200ms对应10帧的音频
    }

    errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
    if (MSP_SUCCESS != errcode)
    {
      printf("\nQISRAudioWrite failed! error code:%d \n", errcode);
    }

    while (MSP_REC_STATUS_COMPLETE != rec_stat)
    {
      const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
      if (MSP_SUCCESS != errcode)
      {
        printf("\nQISRGetResult failed, error code: %d\n", errcode);
      }
      if (NULL != rslt)
      {
        unsigned int rslt_len = strlen(rslt);
        total_len += rslt_len;
        if (total_len >= BUFFER_SIZE)
        {
          printf("\nno enough buffer for rec_result !\n");
          rec_error = "enough buffer for rec_result";
        }
        strncat(rec_result, rslt, rslt_len);
      }
      // usleep(150 * 1000); //防止频繁占用CPU
    }

    // end session
    QISRSessionEnd(session_id, hints);
    printf("(xfyun.cc)>>>>End iat session by id %d.\n", session_id);
    printf("(xfyun.cc)>>>>Result:\n");
    printf("(xfyun.cc)>>>>%s\n", rec_result);
  }
  else
  {
    printf("(xfyun.cc)>>>>Can not start Session: %s", rec_error);
  }

  ret = MSPLogout();
  if (MSP_SUCCESS != ret)
  {
    printf("(xfyun.cc)>>>>MSPLogout failed, error code is: %d", ret);
  }

  if (NULL != f_pcm)
  {
    fclose(f_pcm);
    f_pcm = NULL;
  }

  if (NULL != p_pcm)
  {
    free(p_pcm);
    p_pcm = NULL;
  }

  /**
   * return callback
   */
  Isolate *isolate = info.GetIsolate();
  // callback fn
  Local<Function> cb = Local<Function>::Cast(info[5]);
  const unsigned argc = 2;

  Local<Value> argv[argc] = {String::NewFromUtf8(isolate, rec_error), String::NewFromUtf8(isolate, rec_result)};
  cb->Call(Null(isolate), argc, argv);
}

void Init(Local<Object> exports, Local<Object> module)
{
  exports->Set(Nan::New("iat").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Iat)->GetFunction());
}

NODE_MODULE(xfyun, Init)
} // namespace xfy
