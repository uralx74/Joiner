# Joiner v.1
A simple utility for joining tables in different formats

# Joiner / [English](https://github.com/uralx74/Joiner/tree/master#joiner-en) / [Русский](https://github.com/uralx74/Joiner/tree/master#joiner-ru) 

* [Online Manual](https://github.com/uralx74/Joiner/wiki#joiner-manual) / [Онлайн Руководство Пользователя](http://www.github.com/) 
* [Offline manual](http://www.github.com/) / [Скачать Руководство Пользователя](http://www.github.com/) 
* [FAQ](http://www.github.com/) / [ Часто Задаваемые Вопросы](http://www.github.com/) 


# Joiner [En] 

## Features
* Connects data in different types of sorce like Oracle, Dbf, Excel and Text
* Simple interface
* Uses xml-config files
* Supports command line options
* Supports log files
* Not needs to install

##

### Example xml-config file

#### Excel to Dbf
```
<?xml version="1.0" encoding="windows-1251"?>
<config>
  <import>
      <excel file="..\data\*_import.xlsx" />
  </import>
  <export>
    <dbase4 file="..\data\result.dbf" truncate="true">
      <field name="t" name_src="t" type="C" length="20"/>
    </dbase4>
  </export>
</config>
```

#### Excel to Excel (Multiple files to one file)

```
<?xml version="1.0" encoding="windows-1251"?>
<config>
  <import>
      <excel file="..\data\*_import.xlsx" />
  </import>
  <export>
    <excel file="..\data\result.xlsx" truncate="true">
      <field name="t" name_src="t" format="@"/>
    </excel>
  </export>
</config>
```

Dbf to Oracle throuth Pl/Sql-procedure (Multiple files to one file)
```
<?xml version="1.0" encoding="windows-1251"?>
<config>
  <import>
      <dbase4 file="s:\CMI\Joiner\input_mailbank\bd*.dbf"/>
  </import>
  <export>
    <oraproc server="10.7.0.10:1521:esale" procedure="PK_ADMIN_EDIT.P_MAILBANK_ADD_ED_new" table="nasel_mailbank" truncate="true" code="йrOЫ.%ebяB#&quot;fЧД5">
      <field name="p_nsezon" name_src="nsezon"/>
      <field name="p_summa_e" name_src="summa_e"/>
      <field name="p_p_nd" name_src="p_nd"/>
      <!-- Disabled -->
      <field name="gorod" type="C" length="40" enable="false"/>
    </oraproc>
  </export>
</config>
```

### Example command line
```
joiner -c="..\config\config_xlsx2dbf.xml" -l="..\log\%%d_%%t_joiner_xlsx2dbf.log" -lr -a
```

## OS support
* Windows desktop: 2000, XP, Vista, 7, 8, 8.1, 10
* Windows server: 2003, Home, 2008, 2012, 2012 R2

# Joiner [Ru]

## Особенности
* Connects data in different types of sorce like Oracle, Dbf, Excel and Text
* Обладает простым интерфейсом
* Использует xml-файлы для конфигурирования задания
* Поддерживает параметры командной строки
* Поддерживает журналирование выполнения задания
* Не требует установки
