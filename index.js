/**
 * xfyun client
 */

const xfyun = require('bindings')('xfyun');
var _globalConfig = {}

/**
 * params {username, password, appid, lang, accent, sample_rate}
 * callback callback fn
 */
function _iat(params) {
    // const char* session_begin_params	=	"sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
    return new Promise(function (resolve, reject) {
        xfyun.iat(params.username,
            params.password, // password
            `appid = ${params.appid}`, // login params
            `sub = iat, domain = iat, language = ${params.lang}, accent = ${params.accent}, sample_rate = ${params.sample_rate}, result_type = plain, result_encoding = utf8, aue = speex-wb, ptt = 0, ent = sms-en16k`,
            params.audio_file, // audio file
            function (err, result) {
                if (err)
                    return reject(err);
                return resolve(result);
            });
    });
}

/**
 * params {username, password, appid, lang, accent, sample_rate}
 * callback callback fn
 */
function _awaken(params) {
    // const char *ssb_param = "ivw_threshold=0:-20,sst=wakeup";
    return new Promise(function (resolve, reject) {
        xfyun.awaken(params.username,
            params.password, // password
            `appid = ${params.appid}`, // login params
            `ivw_threshold = 0:-20, sst=wakeup`,
            params.audio_file, // audio file
            function (err, result) {
                if (err)
                    return reject(err);
                return resolve(result);
            });
    });
}

exports.iat = _iat;
exports.awaken = _awaken;