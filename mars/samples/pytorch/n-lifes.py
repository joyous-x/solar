# coding=utf-8
import os
import numpy as np
import torch
from torch.autograd import Variable
from tensorboardX import SummaryWriter
import matplotlib.pyplot as plt
import random

"""
    星期，生命周期（天），次日留存率，3日留存率，7日留存率，
    通过三个留存率预测生命周期
"""

class NNet(torch.nn.Module):
    def __init__(self, input_feature: int):
        super(NNet, self).__init__()
        self.l1 = torch.nn.Linear(in_features=input_feature, out_features=1)
        self.m = torch.nn.Sigmoid()
        self.l2 = torch.nn.Linear(in_features=1, out_features=1)

    def forward(self, inputs: torch.Tensor):
        y = self.l1(inputs)
        y = self.m(y)
        y = self.l2(y)
        return y


class NLifes(object):
    def __init__(self, csv_path, lr=0.0002, logdir="./output/nlife/exp4"):
        self.csv_path = csv_path
        self.writer = SummaryWriter(logdir=logdir, comment="NLifes")
        self.dataset_train, self.dataset_test = self._make_datasets(self._load_datas(self.csv_path))
        self.module = NNet(len(self.dataset_train[0]) - 1)
        self.optimizer = torch.optim.SGD(self.module.parameters(), lr=lr)
        self.loss = torch.nn.MSELoss()

    def _load_datas(self, csv_path):
        lines, datas = [], []
        with open(csv_path) as f:
            lines = f.readlines()
        if len(lines) < 1:
            return datas
        for line in lines[1:]:
            line = line.strip()
            data = line.split(',')
            data = [float(d.strip('%'))/100 if '%' in d else float(d) for d in data]
            datas.append(data)
        return datas

    def draw_rawdatas(self, datas):
        x = [i+1 for i in range(len(datas[0])-2)]
        for i in range(len(datas)):
            y = datas[i][2:]
            plt.plot(x, y)
        plt.show()

    def _make_datasets(self, datasets_raw):
        datasets = []
        for d in datasets_raw:
            datasets.append([d[1], d[2], d[4], d[8]])
        indexs = [i for i in range(len(datasets))]
        train_size = int(len(indexs) * 0.9)
        random.shuffle(indexs)
        dataset_train = datasets[:train_size]
        dataset_test = datasets[train_size:]
        return dataset_train, dataset_test

    def train(self, epochs=100, device=torch.device('cpu'), model_path="./output/nlife/model.pth"):
        for epoch in range(epochs):
            loss_total = 0

            indexs = [i for i in range(len(self.dataset_train))]
            random.shuffle(indexs)

            self.module.train()
            for i,v in enumerate(indexs):
                x, y = self.dataset_train[v][1:], self.dataset_train[v][0]
                x, y = torch.Tensor(np.array(x, dtype=(np.float32))), torch.Tensor(np.array(y, dtype=(np.float32)))
                x, y = Variable(x.to(device)), Variable(y.to(device))
                predicted = self.module(x)

                self.optimizer.zero_grad()
                loss_trained = self.loss(y, predicted)
                loss_trained.backward()
                self.optimizer.step()

                loss_total += loss_trained.item()
                print("epoch: {} step: {}, loss is: {}".format(epoch, i, loss_trained.item()))

            loss_test = self.test(device=device)
            loss_train = loss_total / len(indexs)
            self.writer.add_scalar("loss_train", loss_train, epoch)
            self.writer.add_scalar("loss_test", loss_test, epoch)
            print("epoch: {} loss_train={} loss_test={}".format(epoch, loss_train, loss_test))
        torch.save(self.module.state_dict(), model_path)

    def test(self, device: torch.device):
        self.module.eval()

        loss_total = 0.0
        indexs = [i for i in range(len(self.dataset_test))]
        random.shuffle(indexs)
        for i, v in enumerate(indexs):
            x, y = self.dataset_test[v][1:], self.dataset_test[v][0]
            x, y = torch.Tensor(np.array(x, dtype=(np.float32))), torch.Tensor(np.array(y, dtype=(np.float32)))
            x, y = Variable(x.to(device)), Variable(y.to(device))
            predicted = self.module(x)
            loss_trained = self.loss(y, predicted)
            loss_total += loss_trained.item()
            # print("test: step={}, loss is: {}".format(i, loss_trained.item()))
        return loss_total / len(indexs)

if __name__ == "__main__":
    predictor = NLifes("./datasets/n-lifes/data_sample.csv")
    #predictor.draw_rawdatas(predictor.datas)
    predictor.train(epochs=3000)