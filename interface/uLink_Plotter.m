close all;clear all;clc;

addpath('C:\Users\Dan\Dropbox\Tata_demo_code\uLinkInterface');

fileID = fopen('MicroGrid_Data.txt','r');
A = fscanf(fileID,'%f');

numDataPtsPerReading = 5;
numDataPts = length(A)/numDataPtsPerReading;

outputCurrent =zeros(1,numDataPts);

for i=1:numDataPts    
    outputCurrent(i) = A(i*numDataPtsPerReading);
end

%% Generate solar scenarios

%In Watts




















