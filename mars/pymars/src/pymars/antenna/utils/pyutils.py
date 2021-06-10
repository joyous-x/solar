import subprocess
import os.path
import time
import sys
import os


class PyUtils(object):
    def __init__(self):
        pass

    @staticmethod
    def exec_cmd(self, cmd):
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        return p.returncode, out, err

    @staticmethod
    def tar_x(self, src, target = None):
        """  tar -xvf $src -C $target """
        if target and False == os.path.isdir(target):
            mkdir_cmd = "mkdir -f %s" % target
            self.exec_cmd(mkdir_cmd)
        tar_x_cmd = "tar -xvf %s -C %s" % (src, target)
        ret, out, err = self.exec_cmd(tar_x_cmd)
        return ret, out, err

    @staticmethod
    def cur_time(self):
        return time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime())
    
    @staticmethod
    def add_sys_path(self, path=None):
        if not path:
            path = os.getcwd()
        sys.path.append(path)

