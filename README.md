# xfyun-node
[源模块](https://github.com/Samurais/xfy-node)参考Samurais完善的讯飞接口，实现win和linux跨平台接口及语音唤醒功能，文字转换接口待实现。        
[科大讯飞](http://www.xfyun.cn/)的Node.js SDK。
* 支持 win32,linux，目前仅支持科大讯飞听写和唤醒接口。

* Install
```
npm install node-xfy --save
```

## Usage
```
    const xfy = require('node-xfy');
    let params = {
        username: null, // 账号名称
        password: null, // 账号密码 
        appid: '5864ae2d', // AppID
        // 语言
        // zh_cn:简体中文
        // zh_tw:繁体中文
        // en_us:英语
        // 默认为zh_cn
        lang: 'zh_cn', 
        // 口音
        // mandarin:普通话
        // cantonese:粤语
        // 默认为mandarin
        accent: 'mandarin',
        // 音频格式
        // 8000, 16000, 默认为16000
        sample_rate: 16000,
        // 音频文件位置，绝对路径
        audio_file: 'wav/iflytek01.wav'
    }

    xfy.iat(params)
        .then(function (result) {
            console.log('result', result);
        }, function(err){
            console.log('err', err);
        });
```

## Contribution
```
npm install
node-gyp rebuild
```

## 资源链接
[语音识别科普](http://bbs.xfyun.cn/forum.php?mod=viewthread&tid=6911)

[服务介绍](http://www.xfyun.cn/doccenter/asr)

[开发者论坛](http://bbs.xfyun.cn/portal.php)

[识别英语](http://bbs.xfyun.cn/forum.php?mod=viewthread&tid=22602&extra=page%3D1)

# Copyright & License

Code & Docs 2017© vacant enaldick@hotmail.com

Code released under the ISC license

Docs released under Creative Commons