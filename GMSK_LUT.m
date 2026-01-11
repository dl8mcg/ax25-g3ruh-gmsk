%% =================================================
%% GMSK Gaussian Minimum Shift Keying - DL8MCG
close all; clear all;

%% -------------------------------------------------
% Funktionen zuerst definieren

function b = gaussdesign_oct(bt, span, sps)
    % Zeitachse in Symbolzeiten (wie MATLAB)
    t = -span/2 : 1/sps : span/2;

    % Korrekte Alpha-Definition (aus bt = B*Ts abgeleitet)
    alpha = pi * bt * sqrt(2 / log(2));

    % Gauß-Impulsantwort
    b = exp(-(alpha * t).^2);

    % Normalisierung der Energie (MATLAB normalisiert auf Summe 1)
    b = b / sum(b);
endfunction

function data = datasequencer(n, x1)
    data = reshape(ones(n,1)*x1,1,length(x1)*n);
endfunction

function y2 = interpolate(y)
    x = 1:length(y);
    z = 0.5:0.5:length(y);
    y2 = interp1(x,y,z,'spline');
endfunction

function y2 = convolve(n, x1, b)
    data = datasequencer(n, x1);
    y = conv(b, data);
    y2 = interpolate(y);
endfunction

%% -------------------------------------------------
% Parameter
bt   = 0.5;
span = 3;
sps  = 4;
interp_factor = 2;
Rb = 9600;
fs = Rb * sps * interp_factor;
fprintf('Sample Rate = %d Hz\n', fs);

%% -------------------------------------------------
% Gaussian Filter
b = gaussdesign_oct(bt, span, sps);
n = sps;

%% -------------------------------------------------
% Bitfolgen (Zentrum = 3. Bit)
bit_sequences = {
    [1 0 0 0 0], ...
    [1 0 0 0 1], ...
    [1 0 0 1 0], ...
    [1 0 0 1 1], ...
    [1 0 1 0 0], ...
    [1 0 1 0 1], ...
    [1 0 1 1 0], ...
    [1 0 1 1 1]
};

lut_titles = {'000','001','010','011','100','101','110','111'};

%% -------------------------------------------------
% Faltung + Interpolation
lut_data = cell(1,8);
for k = 1:8
    lut_data{k} = convolve(n, bit_sequences{k}, b);
end

%% -------------------------------------------------
% LUT-Bereich
symbol_offset = 2;    % drittes Bit
lut_len   = sps * interp_factor;
lut_start = round((span + symbol_offset - 0.5) * sps * interp_factor) + 1;
lut_end   = lut_start + lut_len - 1;
fprintf('LUT length = %d samples\n', lut_end - lut_start + 1);

%% -------------------------------------------------
% Plotparameter
lut_height = 1;
lut_color  = [1 0.8 0.8];  % RGB

%% -------------------------------------------------
% Figure 1: alle LUTs als Subplots
figure(1); clf;
for k = 1:8
    subplot(4,2,k); hold on;
    plot(lut_data{k}, 'b', 'LineWidth', 1.5);
    %rectangle('Position',[lut_start,0,lut_len,lut_height], ...
              %'FaceColor',lut_color,'EdgeColor','none');
    rectangle('Position',[lut_start,0,lut_len,lut_height], ...
          'EdgeColor', [1 0.1 0.1], 'LineWidth', 0.8, 'FaceColor', 'none');

    grid on; ylim([0 1]);
    title(['LUT ' lut_titles{k}]); xlabel('Sample'); ylabel('Amplitude');
end

axes('Position',[0 0 1 1],'Visible','off');
text(0.5, 0.98, ['GMSK Lookup Table  –  BT = ' num2str(bt)], ...
     'HorizontalAlignment','center', 'FontSize', 14, 'FontWeight', 'bold');



%% -------------------------------------------------
% LUT abspeichern
folder = 'c:\Users\Hans\ED\DB0INN_MULTI\Octave\';
if ~exist(folder,'dir')
    mkdir(folder);
end

for k = 1:8
    fd = fopen([folder 'LUT' lut_titles{k} '.dac'],'w+');
    for i = lut_start:lut_end
        fprintf(fd,'%4.0f, ', round(lut_data{k}(i) * 255));
    end
    fclose(fd);
end


