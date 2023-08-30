from locust import HttpUser, TaskSet, task
import json

#
# locust -f press_test_by_locust.py --host=http://inner-api.ziao.com
#
# watch -n 0.01 nvidia-smi
#


class TestFaceDetectTask(TaskSet):
    @task
    def req_index(self):
        headers = {
            "Content-Type": "application/json"
        }
        data = {
            "task_name": "inner_detect_frontal_face",
            "image_url": "http://10.12.64.73:6250/files/app/appdata/aicamera_data/sd_digital_role/IMG_0939.JPG",
        }

        r = self.client.post('/aicamera/api/submit', json=data, headers=headers)
        if r.status_code == 200:
           resp_data = json.loads(r.text)
           code = resp_data['code']
           msg = resp_data['msg']
           data = resp_data['resp_data']
           print("===> resp: {} {} {}".format(code, msg, data))
           assert code == 200
        else:
            print("===> fail: ", r.status_code)
        assert r.status_code == 200

# 访问设置
#
# Number of users：设置模拟用户数。（并发）
# Spawn rate ：每秒产生（启动）的虚拟用户数。（递增数）
class ApiTestUser(HttpUser):
    tasks = [TestFaceDetectTask]  # 指向一个定义的用户行为类
    min_wait = 3000  # 执行事务之间用户等待时间的下界（单位：毫秒）
    max_wait = 6000  # 执行事务之间用户等待时间的上界（单位：毫秒）


if __name__ == '__main__':
    user = ApiTestUser()
