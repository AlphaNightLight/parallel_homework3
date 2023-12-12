### Read the files
report_serial =...
        dlmread('reports/report_matMul.csv', ',', 1, 0);
report_rows_strong =...
        dlmread('reports/report_matMulRowsPar_strong.csv', ',', 1, 0);
report_rows_weak =...
        dlmread('reports/report_matMulRowsPar_weak.csv', ',', 1, 0);
report_cols_strong =...
        dlmread('reports/report_matMulColsPar_strong.csv', ',', 1, 0);
report_cols_weak =...
        dlmread('reports/report_matMulColsPar_weak.csv', ',', 1, 0);
report_block_strong =...
        dlmread('reports/report_matMulBlockPar_strong.csv', ',', 1, 0);
report_block_weak =...
        dlmread('reports/report_matMulBlockPar_weak.csv', ',', 1, 0);

num_processors = report_rows_strong(:,1);
report_serial = report_serial(1,4) * ones(length(num_processors),1);
report_rows_strong = report_rows_strong(:,5);
report_rows_weak = report_rows_weak(:,5);
report_cols_strong = report_cols_strong(:,5);
report_cols_weak = report_cols_weak(:,5);
report_block_strong_8 = report_block_strong(1:3:(length(num_processors)*3),7);
report_block_strong_32 = report_block_strong(2:3:(length(num_processors)*3),7);
report_block_strong_128 = report_block_strong(3:3:(length(num_processors)*3),7);
report_block_weak_8 = report_block_weak(1:3:(length(num_processors)*3),7);
report_block_weak_32 = report_block_weak(2:3:(length(num_processors)*3),7);
report_block_weak_128 = report_block_weak(3:3:(length(num_processors)*3),7);



title_names = ['by Rows'; 'by Columns'; 'by Blocks'; 'comparison'];
plot_names = ['rows'; 'cols'; 'block';'comparison'];
for (i_type = 1:4)
  ### Strong Plot
  switch (i_type)
    case 1
      report_strong = report_rows_strong;
    case 2
      report_strong = report_cols_strong;
    case 3
      report_strong = [report_block_strong_8,report_block_strong_32,...
                       report_block_strong_128];
    case 4
      report_strong = [report_rows_strong,report_cols_strong,...
                       report_block_strong_128];
  endswitch
  
  semilogx(num_processors,report_serial,'linewidth',2,...
           num_processors,report_strong,...
           'linewidth',2,'marker','.','markersize',30);

  xticks(num_processors);
  xticklabels(num_processors);
  grid on;
  set(gca,'XMinorTick','Off');
  set (gca, 'xminorgrid', 'off');
  set(gca,'YMinorTick','Off');
  set (gca, 'yminorgrid', 'off');

  title(['Parallel Matrix Multiplication ' strtrim(title_names(i_type,:))...
         ', Strong Scaling']);
  ylabel('time [s]');
  xlabel('num\_threads [n]');
  switch (i_type)
    case 1
      legend('Serial','Row partition','location', 'northeast');
    case 2
      legend('Serial','Column partition','location', 'northeast');
    case 3
      legend('Serial','Blocks 8x8','Blocks 32x32','Blocks 128x128',...
             'location', 'northeast');
    case 4
      legend('Serial','Row partition','Column partition','Blocks 128x128',...
             'location', 'northeast');
  endswitch

  print(['plots/plot_' strtrim(plot_names(i_type,:)) '_strong.png'],'-dpng');



  ### Speedup Plot
  speedup_factor = ones(rows(report_strong),1) * report_strong(1,:);
  report_speedup = speedup_factor ./ report_strong;

  semilogx(num_processors,num_processors,'linewidth',2,...
           num_processors,report_speedup,...
           'linewidth',2,'marker','.','markersize',30);

  xticks(num_processors);
  xticklabels(num_processors);
  grid on;
  set(gca,'XMinorTick','Off');
  set (gca, 'xminorgrid', 'off');
  set(gca,'YMinorTick','Off');
  set (gca, 'yminorgrid', 'off');

  title(['Parallel Matrix Multiplication ' strtrim(title_names(i_type,:))...
         ', Speedup']);
  ylabel('Speedup [n]');
  xlabel('num\_threads [n]');
  switch (i_type)
    case 1
      legend('Ideal','Row partition','location', 'northwest');
    case 2
      legend('Ideal','Column partition','location', 'northwest');
    case 3
      legend('Ideal','Blocks 8x8','Blocks 32x32','Blocks 128x128',...
             'location', 'northwest');
    case 4
      legend('Ideal','Row partition','Column partition','Blocks 128x128',...
             'location', 'northwest');
  endswitch

  print(['plots/plot_' strtrim(plot_names(i_type,:)) '_speedup.png'],'-dpng');



  ### Efficiency Plot
  num_processors_temp = num_processors * ones(1,columns(report_speedup));
  report_efficiency = (report_speedup ./ num_processors_temp) * 100;
  ideal_efficiency = ones(rows(report_speedup),1) * 100;

  semilogx(num_processors,ideal_efficiency,'linewidth',2,...
           num_processors,report_efficiency,...
           'linewidth',2,'marker','.','markersize',30);

  xticks(num_processors);
  xticklabels(num_processors);
  grid on;
  set(gca,'XMinorTick','Off');
  set (gca, 'xminorgrid', 'off');
  set(gca,'YMinorTick','Off');
  set (gca, 'yminorgrid', 'off');

  title(['Parallel Matrix Multiplication ' strtrim(title_names(i_type,:))...
         ', Efficiency']);
  ylabel('Efficiency [%]');
  xlabel('num\_threads [n]');
  switch (i_type)
    case 1
      legend('Ideal','Row partition','location', 'northeast');
    case 2
      legend('Ideal','Column partition','location', 'northeast');
    case 3
      legend('Ideal','Blocks 8x8','Blocks 32x32','Blocks 128x128',...
             'location', 'northeast');
    case 4
      legend('Ideal','Row partition','Column partition','Blocks 128x128',...
             'location', 'northeast');
  endswitch

  print(['plots/plot_' strtrim(plot_names(i_type,:)) '_efficiency.png'],'-dpng');



  ### Weak Plot
  switch (i_type)
    case 1
      report_weak = report_rows_weak;
    case 2
      report_weak = report_cols_weak;
    case 3
      report_weak = [report_block_weak_8,report_block_weak_32,...
                       report_block_weak_128];
    case 4
      report_weak = [report_rows_weak,report_cols_weak,...
                       report_block_weak_128];
  endswitch
  
  semilogx(num_processors,report_serial,'linewidth',2,...
           num_processors,report_weak,...
           'linewidth',2,'marker','.','markersize',30);

  xticks(num_processors);
  xticklabels(num_processors);
  grid on;
  set(gca,'XMinorTick','Off');
  set (gca, 'xminorgrid', 'off');
  set(gca,'YMinorTick','Off');
  set (gca, 'yminorgrid', 'off');

  title(['Parallel Matrix Multiplication ' strtrim(title_names(i_type,:))...
         ', Weak Scaling']);
  ylabel('time [s]');
  xlabel('num\_threads & size\_scale\_ factor [n]');
  switch (i_type)
    case 1
      legend('Serial with size[1]','Row partition','location', 'northwest');
    case 2
      legend('Serial with size[1]','Column partition','location', 'northwest');
    case 3
      legend('Serial with size[1]','Blocks 8x8','Blocks 32x32',...
             'Blocks 128x128','location', 'northwest');
    case 4
      legend('Serial with size[1]','Row partition','Column partition',...
             'Blocks 128x128','location', 'northwest');
  endswitch

  print(['plots/plot_' strtrim(plot_names(i_type,:)) '_weak.png'],'-dpng');
endfor
