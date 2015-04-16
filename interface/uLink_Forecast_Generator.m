%% Generate solar scenarios
close all;clear all;clc;

addpath('C:\Users\Dan\Dropbox\Tata_demo_code\uLinkInterface');

%In Watts

%One for a sunny day
cloudinessIndex=1;
forecastPoints=48;
wattsOfPanel = 200;
meanSolarTime= 11.5;

x=linspace(0,24,forecastPoints);

power = wattsOfPanel*exp(-(x-meanSolarTime).^2/12);

fid = fopen('SunnyDay.txt','w');
fprintf(fid,'%f\n',power);

%plot(x,power);

%One for a dreary day
cloudinessIndex=.3;
forecastPoints=48;
wattsOfPanel = 200;
meanSolarTime= 11.5;

x=linspace(0,24,forecastPoints);

power = wattsOfPanel*exp(-(x-meanSolarTime).^2/12);

fid = fopen('SunnyDay.txt','w');
fprintf(fid,'%f\n',power);

%% Test Scenario Generator

scenarioVal = 7.5;
testScenarios=scenarioVal*ones(1,forecastPoints);

fid = fopen('TestScenarios.txt','w');
fprintf(fid,'%f\n',testScenarios);

6619000






