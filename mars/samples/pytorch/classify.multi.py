# coding=utf-8

import os
import torchvision
import torch
from torch import nn
from torch.autograd import Variable
from torchvision import transforms, datasets, models
from tensorboardX import SummaryWriter
import options
from PIL import Image,ImageDraw
import cv2
import numpy as np


class ClassifyMulti(object):
    def __init__(self, options, lr=1e-3):
        self.opts = options

        self.infer_dataset =  self.opts.data_dir + "/infer"
        self.train_dataset = self.opts.data_dir + "/train"
        self.test_dataset = self.opts.data_dir + "/test"
        self.train_loader = None
        self.test_loader = None
        self.make_dataloader(self.train_dataset, self.test_dataset)

        self.model_vgg = torchvision.models.vgg19(pretrained=True, progress=True)
        for p in self.model_vgg.parameters():
            p.requires_grad = False
        self.model_vgg.classifier = nn.Sequential(
            nn.Linear(in_features=25088, out_features=2048),
            nn.ReLU(),
            nn.Linear(in_features=2048, out_features=512),
            nn.ReLU(),
            nn.Dropout(p=0.6),
            nn.Linear(in_features=512, out_features=10),
            nn.LogSoftmax(dim=1)
        )
        self.model_vgg.to(self.opts.device)

        self.loss = nn.CrossEntropyLoss()
        self.optimizer = torch.optim.SGD(self.model_vgg.parameters(), lr=lr)
        self.writer = SummaryWriter(log_dir=self.opts.log_dir)

    def make_dataloader(self, train_dataset, test_dataset):
        self.train_loader = None
        self.test_loader = None
        self.classes = ('plane', 'car', 'bird', 'cat', 'deer', 'dog', 'frog', 'horse', 'ship', 'truck')

        transform_train = transforms.Compose([
            transforms.RandomCrop(32, padding=4),
            transforms.RandomHorizontalFlip(),
            transforms.ToTensor(),
            transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
        ])
        transform_test = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010))
        ])
        full_dataset = torchvision.datasets.CIFAR10(root='./datasets', train=True, download=False, transform=transform_train)
        train_size = int(0.8 * len(full_dataset))
        test_size = len(full_dataset) - train_size
        train_dataset, test_dataset = torch.utils.data.random_split(full_dataset, [train_size, test_size])
        self.train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=128, shuffle=True, num_workers=2)
        self.test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=128, shuffle=False, num_workers=2)

        infer_dataset = torchvision.datasets.CIFAR10(root='./datasets', train=False, download=False, transform=transform_test)
        self.infer_loader = torch.utils.data.DataLoader(infer_dataset, batch_size=128, shuffle=False, num_workers=2)

    def train(self, train_loader, test_loader, epochs=100, checkpoint_dir=None):
        for epoch in range(epochs):
            train_loss, correct, total = 0, 0, 0
            self.model_vgg.train()
            for batch_idx, (inputs, labels) in enumerate(train_loader):
                inputs, labels = inputs.to(self.opts.device), labels.to(self.opts.device)
                inputs, labels = Variable(inputs), Variable(labels)
                outputs = self.model_vgg(inputs)

                loss_trained = self.loss(outputs, labels)
                self.optimizer.zero_grad()
                loss_trained.backward()
                self.optimizer.step()

                train_loss += loss_trained.data
                _, predicted = torch.max(outputs.data, 1)
                total += labels.size(0)
                correct += predicted.eq(labels.data).to(self.opts.device).sum()
                if batch_idx % 50 == 0:
                    print("train %d (%d)  epoch loss: %.3f acc: %.3f " % (
                        epoch + 1, batch_idx + 1, train_loss / (batch_idx+1), 100 * correct / total))
            self.test(epoch, test_loader, checkpoint_dir)
        self.writer.add_graph(self.model_vgg, input_to_model=None)

    def test(self, epoch, test_loader, model_dir):
        test_loss, correct, total = 0, 0, 0
        self.model_vgg.eval()
        for batch_idx, (inputs, labels) in enumerate(test_loader):
            inputs, labels = inputs.to(self.opts.device), labels.to(self.opts.device)
            inputs, labels = Variable(inputs), Variable(labels)
            outputs = self.model_vgg(inputs)

            test_loss += self.loss(outputs, labels)
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += predicted.eq(labels.data).to(self.opts.device).sum()
        print("test %d epoch loss: %.3f acc: %.3f " % (
                epoch + 1, test_loss / total, 100 * correct / total))

        print("Saving epoch={} ...".format(epoch))
        torch.save(self.model_vgg.state_dict(), os.path.join(model_dir, "model.{}.pt".format(epoch)))

    def infer(self, infer_dataloader, model_path):
        self.model_vgg.load_state_dict(torch.load(model_path))
        self.model_vgg.eval()

        data_transforms = transforms.Compose([])

        for idx, input in enumerate(infer_dataloader):
            img_raw = input
            img_input = data_transforms(img_raw)
            img_input = torch.unsqueeze(img_input, 0)
            outputs = self.model_vgg(Variable(img_input.to(self.opts.device)))
            predicted = torch.max(outputs.data, 1)

            draw = ImageDraw.Draw(img_raw)
            width, height = img_raw.size
            draw.text((40, height - 100), str(predicted), fill=(255, 0, 0))

            img = cv2.cvtColor(np.asarray(img_raw), cv2.COLOR_RGB2BGR)
            cv2.imshow("OpenCV", img)
            img_raw.close()
            if cv2.waitKey() & 0xFF == ord('q'):
                break
            else:
                continue

    def run(self):
        if self.opts.mode == "fit":
            self.train(self.train_loader, self.test_loader, epochs=100, checkpoint_dir=self.opts.checkpoint_dir)
        elif self.opts.mode == "infer":
            self.infer(self.infer_dataset, self.opts.model_path)


if __name__ == "__main__":
    classifier = ClassifyMulti(options.DcOptions().parse())
    classifier.run()
