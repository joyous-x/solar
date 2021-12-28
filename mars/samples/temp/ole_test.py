from oletools.olevba import VBA_Parser,TYPE_OLE,TYPE_OpenXML,TYPE_Word2003_XML,TYPE_MHTML,FileOpenError,OlevbaBaseException
from oletools import rtfobj
import os
import json
import subprocess

"""
vbparser = VBA_Parser("D:\\Workspace\\tmp\\ole\\sample_all\\c3f6a8daa9d582a7c8f29e2616a6b0e0.v.xlsm") #27c073cea1e8748bf487cb18a3950793
for (filevbaname, stream_path, vba_filename, vba_code) in vbparser.extract_macros():
    print('Filename    :', filevbaname)
    print('OLE stream  :', stream_path)
    print('VBA filename:', vba_filename)
    print('VBA vba_code:', vba_code)
"""

def oletools_scan(sample_path, oletool_output_dir):
    g = os.walk(sample_path)
    allmacros = 0
    vba_result_list = []

    count = 0
    for path,dirlist,filelist in g:
        for filename in filelist:
            count = count + 1
            vba_result_dic = {}
            filepath = os.path.join(path,filename)
            try:
                vbparser = VBA_Parser(filepath)
            except OlevbaBaseException:
                print("file is not analyzes")
                continue
            except Exception:
                print("file is NoneType")
                continue
            if vbparser.type is None:
               continue
            if vbparser.detect_vba_macros():
                print("VBA Macros found (%d) %s" % (count, filepath))
                allmacros = allmacros + 1
            for (filevbaname, stream_path, vba_filename, vba_code) in vbparser.extract_macros():
                vba_result_dic["vba_id"] = allmacros
                vba_result_dic["filevbaname"] = filevbaname
                vba_result_dic["stream_path"] = stream_path
                vba_result_dic["vba_filename"] = vba_filename
                vba_result_dic["vba_code"] = vba_code

                _, vba_filename_n = os.path.split(vba_filename)
                newname = filename + "__" + stream_path.replace("/","_") + "__" + vba_filename_n
                newpath = os.path.join(oletool_output_dir, newname)
                if not os.path.exists(oletool_output_dir):
                    os.mkdir(oletool_output_dir)
                try:
                    with open(newpath, "w", encoding="utf-8") as f:
                        f.write(vba_code)
                except Exception as e:
                    print("wirte file error: %s" % (newpath))
                    continue
            if vba_result_dic:
                vba_result_list.append(vba_result_dic.copy())
    try:
        if os.path.exists("result.json"):
            os.remove("result.json")
        if vba_result_list != []:
            with open("result.json", mode="w+", encoding="utf-8") as ij:
                json.dump(vba_result_list, ij)
    except Exception as e:
        print("info.json open fiald!")
    print("共发现" + str(allmacros) + "有问题的样本")


def move_file(filename, src_dir="D:\\Workspace\\tmp\\ole\\compare\\sample\\", dst_dir = "D:\\Workspace\\tmp\\ole\\compare\\ok\\"):
    if os.path.exists(src_dir + filename):
        shutil.move(src_dir + filename, dst_dir + filename)


import shutil
def statics(opath,filepath_a, filepath_b):
    files_counter_a = {}
    files_counter_b = {}
    files_a = os.listdir(filepath_a)
    files_b = os.listdir(filepath_b)
    for f in files_a:
        rst = f.find("__")
        name = f[:rst]
        if name not in files_counter_a:
            files_counter_a[name] = 0
        files_counter_a[f[:rst]] = files_counter_a[f[:rst]] + 1
    for f in files_b:
        rst = f.find("__")
        name = f[:rst]
        if name not in files_counter_b:
            files_counter_b[name] = 0
        files_counter_b[f[:rst]] = files_counter_b[f[:rst]] + 1

    keys = set()
    notaorb = 0
    counta = 0
    countb = 0
    for k in files_counter_a.keys():
        keys.add(k)
    for k in files_counter_b.keys():
        keys.add(k)
    for k in keys:
        if k in files_counter_a and k in files_counter_b:
            count_a = files_counter_a[k]
            count_b = files_counter_b[k]
            if count_a > count_b:
                notaorb = notaorb + 1
                move_file(k, src_dir="D:\\Workspace\\tmp\\ole\\compare\\sample\\",
                          dst_dir="D:\\Workspace\\tmp\\ole\\compare\\bad\\")
                print("%s is not equals in a and b \n" % f)
            else:
                move_file(k)
        elif k in files_counter_a:
            counta = counta + 1
            move_file(k, src_dir="D:\\Workspace\\tmp\\ole\\compare\\sample\\", dst_dir="D:\\Workspace\\tmp\\ole\\compare\\bad\\")
            print("%s is only in %s\n" % (k, filepath_a))
        elif k in files_counter_b:
            countb = countb + 1
            move_file(k)
            print("%s is only in %s\n" % (k, filepath_b))
        else:
            print("somethin unexpected\n")
    print("%d is not a or b!\n" % notaorb)
    print("%d is in a!\n" % counta)
    print("%d is in b!\n" % countb)


def copy_all_exists_samples(src_path, pathb, dst_path):
    src_list = os.listdir(src_path)
    need_list = os.listdir(pathb)
    for i in need_list:
        if i in src_list:
            shutil.copyfile(os.path.join(src_path, i), os.path.join(dst_path, i))


class ForCleanTest(object):
    @staticmethod
    def parser_scan_rst(scan_rst_filepath):
        datas = []
        lines = []
        with open(scan_rst_filepath, "r", encoding="utf-8") as f:
            lines = f.readlines()
        current_filename = current_filepath = None
        for i in range(len(lines)):
            line = lines[i].strip()
            if "=> ready to process" in line:
                fileds = line.split(":", 1)
                current_filepath = fileds[1].strip()
                _, current_filename = os.path.split(current_filepath)
                continue
            if "==== black" not in line:
                continue
            filetype = sid = soffset = root_filetype = None
            fields = line.split()
            for field in fields:
                if "root_filetype=" in field:
                    root_filetype = int(field.strip("root_filetype="))
                elif "did=" in field:
                    sid = int(field.strip("did="))
                elif "offset=" in field:
                    soffset = int(field.strip("offset="))
                elif "filetype=" in field:
                    filetype = int(field.strip("filetype="))
                else:
                    continue
            if filetype and sid and soffset:
                datas.append((current_filename, filetype, sid, soffset, root_filetype, current_filepath))
                print("===> ok: %s" % line)
            else:
                print("===> error: %s" % line)
        return datas

    @staticmethod
    def make_clean_script(scan_rst_filepath, src_path, tmp_path, clean_script_path):
        cmd_fmt = "--cmd=clean --stype=%d --sid=%d --soffset=%d --filepath=\"%s\" --dir_temp=\"%s\""
        cmd_lines = []
        datas = ForCleanTest.parser_scan_rst(scan_rst_filepath)
        for value in datas:
            current_filename, filetype, sid, soffset, _, _ = value
            cmd = cmd_fmt % (filetype, sid, soffset, os.path.join(src_path, current_filename), tmp_path)
            cmd_lines.append(cmd)
            print("===> ok: %s" % (cmd))
        with open(clean_script_path, "w", encoding="utf-8") as f:
            f.writelines([ c+"\n" for c in cmd_lines])

    @staticmethod
    def run_clean_script(tool_name, clean_script_path):
        cmd_lines = []
        with open(clean_script_path, "r", encoding="utf-8") as f:
            cmd_lines = f.readlines()
            cmd_lines = [l.strip() for l in cmd_lines]
        for cmd_line in cmd_lines:
            cmd_line = tool_name + " " + cmd_line
            rst = subprocess.Popen(cmd_line, stdout=subprocess.PIPE, shell=True)
            out, err = rst.communicate()
            rst.wait()
            print(out.decode(), err)

    @staticmethod
    def rename_sample_with_suffix(scan_rst_filepath):
        datas = ForCleanTest.parser_scan_rst(scan_rst_filepath)
        for value in datas:
            current_filename, filetype, sid, soffset, root_filetype, filepath = value
            if not os.path.exists(filepath):
                continue
            if root_filetype == 104:
                suffix = "docx"
            elif root_filetype == 108:
                suffix = "xlsx"
            elif root_filetype == 113:
                suffix = "pptx"
            elif root_filetype == 98:
                suffix = "doc"
            elif root_filetype == 99:
                suffix = "xls"
            elif root_filetype == 100:
                suffix = "ppt"
            else:
                suffix = ""
            if not suffix or len(suffix) < 1:
                continue
            dst_filepath = filepath + "." + suffix
            shutil.move(filepath, dst_filepath)
            print("===> move ok: %s" % (filepath))



if __name__ == "__main__":
    dir_root = "d:\\Workspace\\tmp\\ole\\compare\\"
    #sample_dir = dir_root + "sample\\"
    #oletool_output_dir = dir_root + "oletool_out\\"
    #orenpath = "D:\\Workspace\\tmp\\ole\\compare\\dubalost_1126\\ole_sample_lost\\"
    #oletools_scan(sample_dir, oletool_output_dir)
    #statics(orenpath, oletool_output_dir, "D:\\Workspace\\tmp\\ole\\compare\\mytool_out\\")
    #copy_all_exists_samples("d:\\Workspace\\tmp\\ole\\compare\\clean_test_raw\\", "d:\\Workspace\\tmp\\ole\\compare\\clean_test_cleaned_by_kaba\\", "d:\\Workspace\\tmp\\ole\\compare\\clean_test_new\\")

    ### for clean testing
    scan_rst_filename = "ole_scan_rst.txt"
    scan_sample_dirname = "sample_all\\ole"
    scan_temp_dirname = "mytool_out"
    nexecscan_tools_filepath = "D:\\Workspace\\datatools\\ole_scanner\\Debug\\nexecscan_tools.exe"
    clean_script_filename = "ole_clean_cmd"
    ForCleanTest.make_clean_script(dir_root + scan_rst_filename, dir_root+scan_sample_dirname+"\\", dir_root+scan_temp_dirname+"\\", dir_root + clean_script_filename)
    ForCleanTest.run_clean_script(nexecscan_tools_filepath, dir_root + clean_script_filename)
    #ForCleanTest.rename_sample_with_suffix(dir_root + scan_rst_filename)