import os
import subprocess
import sys
import shutil

buildtools = 'D:\\android_sdk\\build-tools\\23.0.3'
jdk_bin = 'C:\\Program Files\\Java\\jdk1.7.0_79\\bin'

aapt     = buildtools + '\\aapt.exe'
zipalign = buildtools + '\\zipalign.exe'
sign     = jdk_bin + '\\jarsigner.exe'
keytool  = jdk_bin + '\\keytool.exe'

key = 'D:\\test\\test\\key.keystore'

#-------------------------------------------------------------------

# 生成证书：keytool -genkey -alias joyous -keyalg RSA -validity 20000 -keystore key.keystore
# 查看apk签名：keytool -printcert -file META-INF/CERT.RSA
# 查看签名文件：keytool -list -v -keystore D:\test\test\key.keystore

cur_coding = sys.stdout.encoding

#-------------------------------------------------------------------

def getPath(path):
    path,ext = os.path.splitext(path)
    return path

def run(cmd):
    print('CMD---'+cmd)
    output = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    stdout = output.stdout.readlines()
    for line in stdout:
        line = line.decode(cur_coding).strip('\n')
        print(r"    A_S："+line)
    for line in output.stderr.readlines():
        line = line.decode(cur_coding).strip('\n')
        print(r"    A_E："+line)
    print("\r\n")
    return stdout
    
def notes():
    string = ''' add/remove file apk '''
    print(string)

def alignedAPKCheck(apk):
    cmd = zipalign + " -c -v 4 ";
    cmd = cmd + apk
    run(cmd)
    
def checkSign(apk):
    # jarsigner -verify [-verbose] [-certs] apkPath
    cmd = sign + " -verify -verbose " #  -certs
    cmd = cmd + apk
    run(cmd)

def modifyAPK(file, apk, oper):
    # file的分隔符应该用'/' ,否则可能会失败
    output = getPath(apk)+"."+oper+".apk"
    if os.path.exists(output):
        os.remove(output)
    shutil.copy(apk, output)
    if not os.path.exists(output):
        print(output)
        print("modifyAPK: copy failed")
        return ""
    cmd = aapt + " ";
    if oper=="add":
        cmd = cmd + "add "
    else:
        cmd = cmd + "remove "
    cmd = cmd + output + " "
    cmd = cmd + file
    run(cmd)
    return output

def alignedAPK(apk):
    output = getPath(apk)+".aligned.apk"
    if os.path.exists(output):
        os.remove(output)
    cmd = zipalign + " -v 4 ";
    cmd = cmd + apk + " "
    cmd = cmd + output
    run(cmd)
    return output

def addSign2Apk(apk, keystore):
    # jarsigner -verbose -keystore myKeyStore -signedjar TestB_signed.apk TestB.apk myKeyStoreAliasName
    # 在进行签名的时候报错：
    #     必须引用包含专用密钥和相应的公共密钥证书链的有效密钥库密钥条目
    # 原因：
    #     最后应该写的是keystore中的别名而不是keystore文件名
    srcpath = apk
    dstpath = getPath(srcpath) + ".signed.apk"
    if os.path.exists(dstpath):
        os.remove(dstpath)
    keyStroeAliasName = "joyous"
    cmd = sign + " -verbose -keystore"
    cmd = cmd + " " + keystore
    cmd = cmd + " -signedjar "
    cmd = cmd + dstpath + " " + srcpath
    cmd = cmd + " " + keyStroeAliasName
    cmd = cmd + " -storepass " + '666666'
    cmd = cmd + " -keypass " + '666666'
    #cmd = cmd + " -digestalg SHA256 -sigalg MD5withRSA"
    run(cmd)
    return dstpath

def removeSign(apk):
    flag = "META-INF"
    signlist = ""
    cmd = aapt + " list " + apk
    ret = run(cmd)
    for line in ret:
        line = line.decode(cur_coding).strip('\n')
        if flag not in line:
            continue
        signlist = signlist + " " + line
    #print(signlist)
    path = modifyAPK(signlist, apk, "remove")
    return path

#-------------------------------------------------------------------

def addAsset2Apk(assets, path):
    # 修正工作目录
    workdir,name = os.path.split(path)
    os.chdir(workdir)

    path = modifyAPK(assets, path, "add")
    path = removeSign(path)
    path = addSign2Apk(path, key)
    path = alignedAPK(path)
    #alignedAPKCheck(path)
    #checkSign(path)    

if __name__=="__main__":
    path    = "D:\\test\\test\\new.apk"
    assets  = "assets/file_b"
    addAsset2Apk(assets, path)
    
    
