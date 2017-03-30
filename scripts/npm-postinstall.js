/**
 * Created by weiqun on 2017-03-29.
 */
var os = require('os');
var exec = require('child_process').exec;

var destBinPath = "build\\Release\\"

if(os.platform()==="win32"){
    exec('xcopy src\\sdk\\win32\\bin\\* '+destBinPath+' /s/Y',{encoding:"gbk"},function(error,stdout,stderr){
        if(error)console.log('exec exit:',error);
    })
}else if(os.platform()==="linux"){
    exec('cp src\\sdk\\linux\\bin '+destBinPath+' -R',{encoding:"gbk"},function(error,stdout,stderr){
        if(error)console.log('exec exit:',error);
    })
}