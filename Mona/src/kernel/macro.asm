%ifdef BUILD_ON_LINUX
  %macro cglobal 1
            global %1
  %endmacro

  %macro cextern 1
            extern %1
  %endmacro
%else
  %macro cglobal 1
            global _%1
    %define %1 _%1
  %endmacro

  %macro cextern 1
            extern _%1
    %define %1 _%1
  %endmacro
%endif
