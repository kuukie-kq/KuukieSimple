//
// Created by kuu-kie on 2022/7/22.
//

#ifndef VIRTUAL_CPU_H
#define VIRTUAL_CPU_H

// 编译器指令，有没有用看编译器支持不支持
//#pragma once

// notice
// 注意，切忌在自定义的命名空间中引用系统头文件，避免造成标识符的混乱。
// 在多层namespace的情况下存在里层namespace对于系统namespace中的函数错乱，找不到的情况
#include <iostream> //输入输出
#include <sstream> //输入输出缓冲
#include <cstdarg> //不定参数处理
#include <string> //字符串类
#include <vector> //不定数组，字符串分割
#include <fstream> //读写文件
#include <chrono> //时间戳

//system为执行脚本命令与exec区别
//Windows默认代码页936 编码为GBK 说明简体中文 可能与系统语言有关 不是utf-8(65001)编码存在乱码问题
//system("chcp 65001");

namespace kuu {
    //工具，与工具函数不同，不属于必要的，目前只有对于输出进行了简单的封装，还存在问题
    namespace str {
        class system_stream_io {
        private:
            unsigned int on_or_off;
            unsigned int level;
            void step() const;
            void log() const;
            void warning() const;
            void error() const;
            std::stringstream* ss;
            std::stringstream* buff;
        public:
            explicit system_stream_io(unsigned int ooo = 1u, unsigned int mode = 0);
            ~system_stream_io();
            void clear();
            void start(unsigned int mode);
            void maybe(unsigned int mode);
            void append(const char* format, ...) const;
            void end();
        };

        void system_stream_io::step() const {
            if (buff->rdbuf()->in_avail() > 1048575) {
                std::cout << buff->str();
                buff->clear();
                buff->str("");
            }
            *buff << "<->" << ss->str() << "\n";
        }

        void system_stream_io::log() const {
            if (buff->rdbuf()->in_avail() > 0) {
                std::cout << buff->str();
                buff->clear();
                buff->str("");
            }
            std::cout << "<*>" << ss->str() << std::endl;
        }

        void system_stream_io::warning() const {
            std::cout << ">-<" << ss->str() << std::endl;
        }

        void system_stream_io::error() const {
            std::cerr << ">*<" << ss->str() << std::endl;
        }

        system_stream_io::system_stream_io(unsigned int ooo, unsigned int mode) {
            //如果仅仅作为局部变量，输出完就释放内存
            //可以将第二个参数填上
            //不需要进行start或者maybe即可输出
            //
            //作为频繁的步骤，输出很影响性能
            //不完全数据
            //100040002次循环
            //中间过程不输出2s完成
            //中间过程全部每次输出900~1000s完成
            //加入buff缓冲，全输出但并不是实时的依据设置的大小不同400~700s完成
            //所以，保留buff
            ss = new std::stringstream();
            buff = new std::stringstream();
            on_or_off = ooo;
            level = mode;
        }

        system_stream_io::~system_stream_io() {
            delete ss;
            delete buff;
        }

        void system_stream_io::clear() {
            ss->clear();
            ss->str("");
            level = 0;
        }

        void system_stream_io::start(unsigned int mode) {
            //当对象为全局的时候，输出的起始
            //作为不频繁申请内存
            if (ss->rdbuf()->in_avail() > 0) {
                clear();
            }
            level = mode;
        }

        void system_stream_io::maybe(unsigned int mode) {
            //局部时，可能不确定在那一个地方进行了设置
            //但又不想在正常情况下输出空行
            level = mode;
        }

        void system_stream_io::append(const char* format, ...) const {
            //不定参数的一种实现
            //目前来说比较方便的
            if (level & on_or_off) {
                char *buffer = (char *) malloc(1024);
                va_list args;
                va_start(args, format);
                int length = vsnprintf(buffer, 1024, format, args);
                (length >= 1024) ? *ss << length << "- format and args too long" : *ss << buffer;
                va_end(args);
                delete buffer;
            }
        }

        void system_stream_io::end() {
            if (level == 0) return;
            bool error_on = (level & 1u) & (on_or_off & 1u);
            bool warning_on = (level & 2u) & (on_or_off & 2u);
            bool log_on = (level & 4u) & (on_or_off & 4u);
            bool step_on = (level & 8u) & (on_or_off & 8u);

            if (error_on) {
                error();
            } else if (warning_on) {
                warning();
            } else if (log_on) {
                log();
            } else if (step_on) {
                step();
            }
            clear();
        }
    }

    //考虑使用匿名命名空间，只在本文件内可见
    namespace cpu {
        //.h文件部分
        //基本所有函数如果无特殊用途的返回值类型均为int，其中1代表正常退出，0代表异常
        class [[maybe_unused]] tagscccc;//定义部分
        //类型定义
        //统一类型，只有所占字节大小不同，都按整数类型进行处理
        typedef unsigned long u8int;
        typedef unsigned int u4int;
        typedef unsigned short u2int;
        typedef unsigned char u1int;
        [[maybe_unused]] typedef long s8int;
        typedef int s4int;
        typedef short s2int;
        [[maybe_unused]] typedef char s1int;
        //工具函数，计算hash值和字符串的分割
        u4int hash_code(const char* key);
        std::vector<std::string> spilt(std::string str, const std::string& pattern);
        //工具类

        //时间戳
        //C++11 chrono库
        //使用的最高精度，nanoseconds纳秒
        //格式化输出方式
        //先将时间戳转换成chrono::time_point或者chrono::system_clock::time_point
        //然后就可以使用chrono::system_clock::to_time_t转成std::time_t
        //既ctime库中的使用方式
        class time_stamp;
        //成员类

        // 说明
        // 内存地址大小 1 16 16 16 16 16 16 16 16 32 32 32 32 64 64 128 511
        // 总共1024，每个可以存放32位数据
        // 使用方法
        // 内存先申请，再使用，用完释放
        // 并且对于read不做限制，释放也只是设置使用大小为0，内容不变
        // 模拟申请内存后，如果不进行初始化，出现奇怪的数
        class virtual_memory;
        // 说明
        // 函数名与内存中首地址的键值对映射
        // 虽然为键值对模型，但时间换空间，空间换时间的思想，没有使用map结构
        // 目前使用最简单的链表结构
        // 考虑用红黑树替代
        class function_member;
        // 说明
        // 函数调用栈
        // 目前无限制，加个size限制栈大小
        // 使用方法
        // 压栈弹栈，函数名采用hash存储
        class function_stack;
        // 说明
        // 寄存器组
        // ax bx cx dx e1x e2x e3x e4x e5x e6x e7x e8x e9x rx fx
        // 对应数组顺序，考虑到hashmap也是使用数组实现的
        // 不对应需加一个键值对映射，键为寄存器名值为index
        // 使用方法
        // 从寄存器中取值和存值
        class virtual_register;
        // 说明
        // 编码树，统一存放指令对应函数的树
        // 与二进制指令相对应，0左1右
        // 使用方法
        // 绑定函数以及查找函数，其中有位运算，用于计算二进制对应位置的值
        class coding_tree;
        //主类
        // 说明
        // 主类，CPU解释器的执行，模拟CPU按照机器码做出相应动作
        // 使用方法
        // 先读取，读取汇编翻译成机器码，后面的CPU调度树，通过读取切换，既内容逻辑复杂
        // 然后按步骤执行，pc计数器用于记录总共执行了多少个机器码
        // 结果输出，相当于测试用例仅用于查看一些成员对象中的数据是否符合预期
        class counter_step;
        //指令对应的函数
        using function = s4int (*)(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int mov(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int inc(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int dec(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int jnz(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int jny(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int cal(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        s4int ret(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs);
        // end 七进制分别为C，D，E，F，G(7)，A，B，标识7s
        class [[maybe_unused]] tagscccc {};







        //.cpp文件部分
        class [[maybe_unused]] tagscccd;//实现部分
        u4int hash_code(const char* key) {
            u4int result;
            u1int* byte;
            for (result = 0, byte = (u1int*)key; *byte; byte++) {
                result = result * 31 + *byte;
            }
            return result;
        }

        std::vector<std::string> spilt(std::string str, const std::string& pattern) {
            std::string::size_type pos;
            std::vector<std::string> result;
            str += pattern;  //扩展字符串以方便操作
            u4int size = str.size();
            for (u4int i = 0; i < size; i++) {
                pos = str.find(pattern, i);
                if (pos < size) {
                    std::string s = str.substr(i, pos - i);
                    result.push_back(s);
                    i = pos + pattern.size() - 1;
                }
            }
            return result;
        }

        class time_stamp {
            //long long 64位
            //18446744073709551616 2^64 20位
            //9999999999000000000 纳秒时间戳 2286-11-21 1:46:39 最大？
            //unsigned long long 目前够用
        private:
            std::chrono::nanoseconds start_stamp = std::chrono::nanoseconds::zero();
        public:
            time_stamp();
            ~time_stamp();
            u8int time_line_passed();
        };

        time_stamp::time_stamp() {
            start_stamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
            );
        }

        time_stamp::~time_stamp() {
            start_stamp = std::chrono::nanoseconds::zero();
        }

        u8int time_stamp::time_line_passed() {
            std::chrono::nanoseconds now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
            );
            u8int passed = now.count() - start_stamp.count();
            if(passed > 0) {
                return (passed);
            }
            return 0;
        }

        class virtual_memory {
            //作为CPU自己的小cache
            //参考分页系统的内存思想，目前只用最简单的方式实现
        private:
            u1int use[16]{};
            u2int index[16]{};
            u4int memory[1024]{};
        public:
            virtual_memory();
            ~virtual_memory();
            u4int apply(u4int size);
            s4int release(u4int segment);
            s4int overlay(u4int segment, u4int offset, u4int value);
            u4int read(u4int segment, u4int offset) const;
        };

        virtual_memory::virtual_memory() {
            for(u1int& i : use) {
                i = 0;
            }
            index[0] = 1;
            index[1] = 17;
            index[2] = 33;
            index[3] = 49;
            index[4] = 65;
            index[5] = 81;
            index[6] = 97;
            index[7] = 113;
            index[8] = 129;
            index[9] = 161;
            index[10] = 193;
            index[11] = 225;
            index[12] = 257;
            index[13] = 321;
            index[14] = 385;
            index[15] = 513;
            for (u4int& i : memory) {
                i = 0u;
            }
            // magic number
            memory[0] = 3260681415u;
        }

        virtual_memory::~virtual_memory() {
            // 只改变使用的部分
            // 参考内存申请之后不初始化出现奇怪的现象
            for(u1int& i : use) {
                i = 0;
            }
        }

        u4int virtual_memory::apply(const u4int size) {
            if (size == 0) { return 0u; }
            if (size <= 1u << 4u) {
                for (s4int i = 0; i < 8; i++) {
                    if (use[i] == 0) {
                        use[i] = size;
                        return index[i];
                    }
                }
                return index[15];
            } else if (size <= 1u << 5u) {
                for (s4int i = 0; i < 4; i++) {
                    if (use[8 + i] == 0) {
                        use[8 + i] = size;
                        return index[8 + i];
                    }
                }
                return index[15];
            } else if (size <= 1u << 6u) {
                for (s4int i = 0; i < 2; i++) {
                    if (use[12 + i] == 0) {
                        use[12 + i] = size;
                        return index[12 + i];
                    }
                }
                return index[15];
            } else if (size <= 1u << 7u) {
                if (use[14] == 0) {
                    use[14] = size;
                    return index[14];
                }
                return index[15];
            }
            return 0u;
        }

        s4int virtual_memory::release(const u4int segment) {
            for (s4int i = 0; i < 16; i++) {
                if (index[i] == segment) {
                    use[i] = 0;
                    return 1;
                }
            }
            return 0;
        }

        s4int virtual_memory::overlay(const u4int segment, const u4int offset, const u4int value) {
            // 写权限校验，无权限控制只对是否能够访问做限制
            for (s4int i = 0; i < 16; i++) {
                if (index[i] == segment) {
                    if (use[i] > offset) {
                        memory[segment + offset] = value;
                        return 1;
                    }
                    break;
                }
            }
            return 0;
        }

        u4int virtual_memory::read(const u4int segment, const u4int offset) const {
            return memory[segment + offset];
        }

        class function_member {
            //仅简单的使用链表进行实现
            //红黑树参考red_black_tree
            //目前想法，需要一个统一接口，作为红黑树中的value
            //可以作为一个单独的项目，放入工具命名空间中
        private:
            class function_member_node {
            public:
                u2int name;
                u4int value;
                function_member_node* next;
                function_member_node();
                ~function_member_node();
            };
            function_member_node* main;
            s2int hash(const char* name);
        public:
            function_member();
            ~function_member();
            s4int load(const char* name, u4int segment);
            s4int unload(const char* name);
            u4int find(const char* name);
        };

        function_member::function_member_node::function_member_node() {
            name = 0;
            value = 0;
            next = nullptr;
        }

        function_member::function_member_node::~function_member_node() {
            delete next;
        }

        s2int function_member::hash(const char* name) {
            main->name; // NOLINT
            s2int result;
            u1int* byte;
            for (result = 0, byte = (u1int*)name; *byte; byte++) {
                result = result * 31 + *byte;
            }
            u2int key = (u2int)result << 1u;
            return key >> 1u;
        }

        function_member::function_member() {
            main = new function_member_node();
        }

        function_member::~function_member() {
            delete main;
        }

        s4int function_member::load(const char* name, u4int segment) {
            function_member_node* node = main;
            for (;node->next != nullptr; node = node->next) {
                if (node->name == hash(name)) {
                    if (node->value == segment) { return 0; }
                    return 0;
                }
            }
            node->next = new function_member_node();
            node->next->name = hash(name);
            node->next->value = segment;
            return 1;
        }

        s4int function_member::unload(const char* name) {
            function_member_node* node = main->next;
            function_member_node* temp = main;
            for (;temp->next != nullptr; temp = temp->next) {
                if (node->name == hash(name)) {
                    break;
                }
                node = node->next;
            }
            if (node != nullptr && temp->next == node) {
                temp->next = node->next;
                node->next = nullptr;
            }
            delete node;
            return 1;
        }

        u4int function_member::find(const char* name) {
            function_member_node* node = main;
            for (;node->next != nullptr; node = node->next) {
                if (node->name == hash(name)) {
                    return node->value;
                }
            }
            if (node->name == hash(name)) {
                return node->value;
            }
            return 0;
        }

        class function_stack {
            //采用链表结构，插入和删除都是在头结点操作，实现栈的后入先出规则
            //name可能仅作为调试时所用
            //使用的32位hash
        private:
            class function_stack_node {
            public:
                u4int name;
                u4int segment;
                function_stack_node* next;
                function_stack_node();
                ~function_stack_node();
            };
            function_stack_node* main;
        public:
            function_stack();
            ~function_stack();
            s4int entrance(u4int segment) const;
            u4int exit() const;
        };

        function_stack::function_stack_node::function_stack_node() {
            name = hash_code("main");
            segment = 0;
            next = nullptr;
        }

        function_stack::function_stack_node::~function_stack_node() {
            delete next;
        }

        function_stack::function_stack() {
            main = new function_stack_node();
        }

        function_stack::~function_stack() {
            delete main;
        }

        s4int function_stack::entrance(const u4int segment) const {
            auto* node = new function_stack_node();
            node->name = 0;
            node->segment = segment;

            node->next = main->next;
            main->next = node;
            return 1;
        }

        u4int function_stack::exit() const {
            function_stack_node* node = main->next;
            if (node == nullptr) { return 0u; }
            main->next = node->next;
            node->next = nullptr;
            node->name; // NOLINT
            u4int segment = node->segment;
            delete node;
            return segment;
        }

        class virtual_register {
            //作为CPU中的寄存器组
            //考虑到硬件中的CPU访问寄存器，采用数组以下标的方式进行访问
        private:
            u4int array[15]{};
        public:
            virtual_register();
            ~virtual_register();
            s4int set(s4int index, u4int value);
            u4int get(s4int index) const;
        };

        virtual_register::virtual_register() {
            for (u4int& i : array) {
                i = 0;
            }
        }

        virtual_register::~virtual_register() {
            for (u4int& i : array) {
                i = 0;
            }
        }

        s4int virtual_register::set(const s4int index, const u4int value) {
            array[index] = value;
            return 1;
        }

        u4int virtual_register::get(const s4int index) const {
            return array[index];
        }

        class coding_tree {
            //二叉树结构，主要是保存统一的指令操作函数
            //存在中间结点浪费问题
        private:
            class coding_tree_node {
            public:
                function operation;
                coding_tree_node* left;
                coding_tree_node* right;
                coding_tree_node();
                ~coding_tree_node();
            };
            coding_tree_node* base;
            bool bit_one(u1int code, u4int index_positive) const;
        public:
            coding_tree();
            ~coding_tree();
            s4int generation(u1int code, function fun) const;
            function found(u4int code) const;
        };

        coding_tree::coding_tree_node::coding_tree_node() {
            operation = nullptr;
            left = nullptr;
            right = nullptr;
        }

        coding_tree::coding_tree_node::~coding_tree_node() {
            delete left;
            delete right;
        }

        coding_tree::coding_tree() {
            base = new coding_tree_node();
        }

        coding_tree::~coding_tree() {
            delete base;
        }

        bool coding_tree::bit_one(const u1int code, const u4int index_positive) const {
            base; // NOLINT
            return code & 1u << (index_positive - 1);
        }

        s4int coding_tree::generation(const u1int code, const function fun) const {
            coding_tree_node* node = base;
            for (s4int i = 3; i > 0; i--) {
                if (bit_one(code, i)) {
                    if (node->right == nullptr) {
                        node->right = new coding_tree_node();
                    }
                    node = node->right;
                } else {
                    if (node->left == nullptr) {
                        node->left = new coding_tree_node();
                    }
                    node = node->left;
                }
            }
            node->operation = fun;
            return 1;
        }

        function coding_tree::found(const u4int code) const {
            u1int first = code >> 24u;
            coding_tree_node* node = base;
            for (s4int i = 3; i > 0; i--) {
                if (bit_one(first, i)) {
                    node = node->right;
                } else {
                    node = node->left;
                }
                if (node == nullptr) {
                    return nullptr;
                }
            }
            return node->operation;
        }

        class counter_step {
            //主体
            //pc程序计数器，记录一个生命周期里所执行的指令数
            //cs当前访问指令的起始位置，调用函数时修改此值，即可实现跳转
            //nc访问指令的偏移，用于获取下一跳指令
        private:
            s4int pc;
            u4int cs;
            u4int nc;
            virtual_register* ncr;
            virtual_memory* vmm;
            function_stack* scf;
            coding_tree* ins;
            function_member* fm;
            u4int constant(const std::string& str) const;
            u4int only_name(const std::string& str) const;
            u4int calculate(s4int index, const std::string& word) const;
        public:
            counter_step();
            ~counter_step();
            s4int read(const char* name, u4int length = 0, const char* file_or_str = nullptr);
            s4int interlude(const char* name = "main");
            s4int step_by_step();
            s4int result() const;
        };

        u4int counter_step::constant(const std::string &str) const {
            pc; // NOLINT
            std::string sta[22] = {
                    "mov", "inc", "dec", "jnz", "jny", "cal", "ret",
                    "ax", "bx", "cx", "dx",
                    "e1x", "e2x", "e3x", "e4x", "e5x", "e6x", "e7x", "e8x", "e9x",
                    "rx", "fx"
            };
            u4int stb[22] = {
                    8u, 9u, 10u, 12u, 13u, 14u, 15u,
                    129u, 130u, 131u, 132u,
                    133u, 134u, 135u, 136u, 137u, 138u, 139u, 140u, 141u,
                    142u, 143u
            };
            for (int i = 0; i < 22; i++) {
                if (sta[i] == str) {
                    return stb[i];
                }
            }
            return 0u;
        }

        u4int counter_step::only_name(const std::string &str) const {
            return fm->find(str.c_str());
        }

        u4int counter_step::calculate(const s4int index, const std::string& word) const {
            if (constant(word) == 0 && index == 2) {
                if (only_name(word) != 0) { return only_name(word); }
                s4int number = std::stoi(word);
                if (number < 0) {
                    return 0u;
                } else {
                    return (u4int)number;
                }
            } else {
                u4int number = constant(word);
                if (index == 0) {
                    return number << 24u;
                } else if (index == 1) {
                    return number << 16u;
                } else {
                    return number << 8u;
                }
            }
        }

        counter_step::counter_step() {
            pc = 0;
            cs = 1;
            nc = 0;
            ncr = new virtual_register();
            vmm = new virtual_memory();
            scf = new function_stack();
            ins = new coding_tree();
            ins->generation(8u, mov);
            ins->generation(9u, inc);
            ins->generation(10u, dec);
            ins->generation(12u, jnz);
            ins->generation(13u, jny);
            ins->generation(14u, cal);
            ins->generation(15u, ret);
            fm = new function_member();
        }

        counter_step::~counter_step() {
            delete ncr;
            delete vmm;
            delete scf;
            delete ins;
        }

        s4int counter_step::read(const char* name, const u4int length, const char* file_or_str) {
            if (fm->find(name) != 0) { return 0; }
            std::string str;
            u4int segment = 0;
            if (file_or_str == nullptr) {
                if (length == 15) {
                    str = "mov e1x 12\n"
                          "mov e2x 31\n"
                          "mov e3x 5\n"
                          "mov e4x 36\n"
                          "mov ax e1x\n"
                          "mov bx e2x\n"
                          "cal 0 17\n"
                          "mov e5x rx\n"
                          "mov ax e3x\n"
                          "mov bx e4x\n"
                          "cal 0 17\n"
                          "mov ax e5x\n"
                          "mov bx rx\n"
                          "cal 0 17\n"
                          "mov e6x rx";
                    segment = vmm->apply(length);
                } else if (length == 9) {
                    str = "mov e1x ax\n"
                          "mov e2x bx\n"
                          "cal 0 33\n"
                          "mov dx rx\n"
                          "inc e1x\n"
                          "dec e2x\n"
                          "jnz e2x 2\n"
                          "mov rx e1x\n"
                          "ret";
                    segment = vmm->apply(length);
                } else if (length == 11) {
                    str = "mov e1x ax\n"
                          "mov e2x bx\n"
                          "dec ax\n"
                          "dec bx\n"
                          "jny ax 2\n"
                          "jny bx 4\n"
                          "jnz bx 4\n"
                          "mov rx e1x\n"
                          "ret\n"
                          "mov rx e2x\n"
                          "ret";
                    segment = vmm->apply(length);
                }
            } else {
                if (length == 0) {
                    str = "";
                    std::ifstream in_read(file_or_str, std::ios::in);
                    s4int number = 0;
                    for (std::string line; std::getline(in_read, line);) {
                        number++;
                        str.append(line).append("\n");
                    }
                    segment = vmm->apply(number);
                } else {
                    str = file_or_str;
                    segment = vmm->apply(length);
                }
            }
            if (segment == 0) { return 0; }
            // 真正的主要部分，注意stoi可能抛出异常，出现说明有地方写错了，不做捕获处理（编译器做好后，不会出现这类问题，出现说明编译器有bug）
            std::vector<std::string> lines = spilt(str, "\n");
            for (s4int i = 0; i < lines.size(); i++) {
                std::vector<std::string> words = spilt(lines[i], " ");
                u4int code = 0;
                for (s4int j = 0; j < words.size(); j++) {
                    code = code + calculate(j,words[j]);
                }
                vmm->overlay(segment, i, code);
            }
            // 函数名处理，只做了15位
            fm->load(name, segment);
            if (fm->find("main") != 0) { cs = fm->find("main"); nc = 0u; }
            return 1;
        }

        s4int counter_step::interlude(const char* name) {
            u4int segment = fm->find(name);
            fm->unload(name);
            if (segment != 0) {
                return vmm->release(segment);
            }
            return 0;
        }

        s4int counter_step::step_by_step() {
            if (pc >= 0 && pc < 2000000000) {
                u4int code = vmm->read(cs, nc);
                if (code == 0) { return 0; }
                function fun = ins->found(code);
                if (fun == nullptr) { return 0; }
                if (fun(nc, code, ncr, vmm, scf, cs) == 0) { return 0; }
                pc++;
                return 1;
            } else {
                return 0;
            }
        }

        s4int counter_step::result() const {
//            std::printf("ありがとうごじゃいます\n");
//            std::printf("pc = [0x%08X(%d)]\n", pc, pc);
//            std::printf("[0x%08X(%d)]\n", ncr->get(2), ncr->get(2));
//            std::printf("[0x%08X(%d)]\n", ncr->get(3), ncr->get(3));
            auto* out = new str::system_stream_io(15u,8u);
            out->append("ありがとうごじゃいます");
            out->end();
            out->start(8u);
            out->append("pc = [0x%08X(%d)]\tcx [0x%08X(%d)]\tdx [0x%08X(%d)]", pc, pc, ncr->get(2), ncr->get(2), ncr->get(3), ncr->get(3));
            out->end();
            out->start(8u);
            out->append("目前，15个寄存器中，规定的用法：rx为固定的返回值寄存器（非必要不做其他用处），fx为预留的调用函数，前面的4个寄存器则作为参数使用，局部变量寄存器使用中间的9个寄存器。因为目前实现的关系，非引用传参任何寄存器都可以。");
            out->end();
            out->start(4u);
            out->append("間違いはありませんが、異常がないかどうかは再確認が必要です");
            out->end();
            delete out;
            return 1;
        }

        s4int mov(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs) {
            u4int first = code >> 24u;
            u4int second = (code << 8u) >> 24u;
            u4int third = (code << 16u) >> 16u;

            if (first != 8u) { return 0; }

            if (third > 1u << 15u) {
                third = third >> 8u;
                ncr->set(second - 129, ncr->get(third - 129));
            } else {
                ncr->set(second - 129, third);
            }
            nc++;
            return 1;
        }

        s4int inc(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs) {
            u4int first = code >> 24u;
            u4int second = (code << 8u) >> 24u;

            if (first != 9u) { return 0; }

            ncr->set(second - 129, ncr->get(second - 129) + 1);
            nc++;
            return 1;
        }

        s4int dec(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs) {
            u4int first = code >> 24u;
            u4int second = (code << 8u) >> 24u;

            if (first != 10u) { return 0; }

            ncr->set(second - 129, ncr->get(second - 129) - 1);
            nc++;
            return 1;
        }

        s4int jnz(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs) {
            u4int first = code >> 24u;
            u4int second = (code << 8u) >> 24u;
            u4int third = (code << 16u) >> 16u;

            if (first != 12u) { return 0; }

            if (ncr->get(second - 129) == 0) {
                nc++;
            } else {
                nc = nc - third;
            }
            return 1;
        }

        s4int jny(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs) {
            u4int first = code >> 24u;
            u4int second = (code << 8u) >> 24u;
            u4int third = (code << 16u) >> 16u;

            if (first != 13u) { return 0; }

            if (ncr->get(second - 129) == 0) {
                nc++;
            } else {
                nc = nc + third;
            }
            return 1;
        }

        s4int cal(u4int& nc, u4int code, virtual_register* ncr, virtual_memory* mem, function_stack* scf, u4int& cs) {
            u4int first = code >> 24u;
            u4int second = (code << 8u) >> 24u;
            u4int third = (code << 16u) >> 16u;

            if (first != 14u) { return 0; }

            nc++;
            u4int segment = mem->apply(12u);
            scf->entrance(segment);
            mem->overlay(segment, 1, cs);
            mem->overlay(segment, 2, nc);
            mem->overlay(segment, 3, ncr->get(4));
            mem->overlay(segment, 4, ncr->get(5));
            mem->overlay(segment, 5, ncr->get(6));
            mem->overlay(segment, 6, ncr->get(7));
            mem->overlay(segment, 7, ncr->get(8));
            mem->overlay(segment, 8, ncr->get(9));
            mem->overlay(segment, 9, ncr->get(10));
            mem->overlay(segment, 10, ncr->get(11));
            mem->overlay(segment, 11, ncr->get(12));

            if (second == 0) { cs = third; }
            nc = 0;
            return 1;
        }

        s4int ret(u4int& nc, const u4int code, virtual_register* ncr, virtual_memory* const mem, function_stack* const scf, u4int& cs) {
            u4int first = code >> 24u;

            if (first != 15u) { return 0; }

            u4int segment = scf->exit();
            cs = mem->read(segment,1);
            nc = mem->read(segment,2);
            ncr->set(4,mem->read(segment,3));
            ncr->set(5,mem->read(segment,4));
            ncr->set(6,mem->read(segment,5));
            ncr->set(7,mem->read(segment,6));
            ncr->set(8,mem->read(segment,7));
            ncr->set(9,mem->read(segment,8));
            ncr->set(10,mem->read(segment,9));
            ncr->set(11,mem->read(segment,10));
            ncr->set(12,mem->read(segment,11));
            mem->release(segment);
            return 1;
        }
        class [[maybe_unused]] tagscccd {};
        //notice
        //CLang-Tidy属于静态代码扫描，语法要求高，但有时却需要那种写法 可以使用//NOLINT屏蔽
    }

    //红黑树的标准实现，node为标准的仅有一个int类型的值
    namespace rbt {
        class red_black_tree {
            // 定义
            // 红黑树是特殊的二叉查找树（AVL）
            // 特性
            // 1）每个节点只有可能为黑色或者红色
            // 2）根节点为黑色
            // 3）叶子节点一定是null，即为黑色
            // 4）如果一个节点是红色，则它的子节点一定是黑色
            // 5）从一个节点到其叶子节点的所有路径包含相同数量的黑色节点
            //相关情况分析及图示
            //https://www.jianshu.com/p/4cd37000f4e3?u_atoken=cdb08e27-1c98-4c4c-a25c-4fbfeb095fa4&u_asession=01ab7xwtg0aZPT-a2o1gat7Mu4ZyIHYXcIxYHdUixTj9qYONbhOgdeNI58HN7RqIWxX0KNBwm7Lovlpxjd_P_q4JsKWYrT3W_NKPr8w6oU7K_uJHWI8z0lFtp0aK1dbNDIE3kHBSeC3ycZ3R-DrPYY52BkFo3NEHBv0PZUm6pbxQU&u_asig=05tPpdLjm4H_veLABeXfEOOcJS5worn3f6CN5tledCQgFfDVi6VSXThHJmziFdfCbe1xEg18CBVN0kQgCY_rmZdunlpetzZIipwTmh8C_agdtp2RoqVBDNZgAveEH6iXtdv3LN_A-oFt43NE19ZET_E9-yK0_zv7zfxz5jr2naAMX9JS7q8ZD7Xtz2Ly-b0kmuyAKRFSVJkkdwVUnyHAIJzdZ24YjhKA_sNukOojR0yiZpLiW964lGVQm7ziqrI2TSWrnxlgSSvoRYzeD7jclHWe3h9VXwMyh6PgyDIVSG1W8Y0HW3Zh6j6ePzqJNxifPBfm5fsg5wmy8bWufzsZqoZDDGBggA94sIlA8ogHcLTxoCN1fL-Q0LRn6kQJGijjnbmWspDxyAEEo4kbsryBKb9Q&u_aref=84kyqG9liWiBspbY%2FI3QVFpYvss%3D
        private:
            enum red_black {
                RED = 0, BLACK = 1
            };

            class red_black_tree_node {
                // Linux内核中的红黑树，color和parent合为一个long指针
                // 因为在系统中的地址，最后两位一定是0（一般情况）
            public:
                int value;
                red_black color;
                red_black_tree_node *left;
                red_black_tree_node *right;
                void *parent;
                explicit red_black_tree_node(int number = 0);
                ~red_black_tree_node();
            };

            // 左旋
            void left_rotate(red_black_tree_node *pivot);

            // 右旋
            void right_rotate(red_black_tree_node *pivot);

            // 为了确保树不会循环（调试查看时绕）同时也为内核实现的方向提供了接口
            red_black_tree_node *parent_node(red_black_tree_node *child) const;

            // 插入后的调整
            void fix_after_append(red_black_tree_node *node);

            red_black_tree_node *found(int value);

            // 查找当前节点的后继节点，既中序遍历的下一个节点
            red_black_tree_node *successor(red_black_tree_node *node) const;

            // 删除时的调整
            void fix_after_subtract(red_black_tree_node *node);

            // 了解内联函数
            void pre_order(unsigned int index, unsigned int offset, red_black_tree_node *node, unsigned int level,
                           str::system_stream_io *out) const;

            red_black_tree_node *root;
        public:
            red_black_tree();

            ~red_black_tree();

            // 插入节点
            // 1）父节点是黑的，无需做处理
            // 2）父节点和叔节点为红，修改父节点和叔节点为黑色，改变祖父节点为红色
            // 3）父节点为红叔节点为黑，四种情况，有直接旋转变色和先反向旋转再旋转变色之分
            void append(int value);

            // 删除节点
            // 1）删除的节点为叶子节点，可以直接删除
            // 2）删除的节点只有一个子节点，子节点顶替
            // 3）删除的节点有两个子节点，先找其后继节点，然后复制内容，则删除的是（替身）后继节点所在的位置
            int subtract(int value);

            void scanning() const;
        };

        red_black_tree::red_black_tree_node::red_black_tree_node(const int number) {
            value = number;
            color = RED;
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }

        red_black_tree::red_black_tree_node::~red_black_tree_node() {
            delete left;
            delete right;
            // 注意如果 delete (red_black_tree_node*)parent;
            // 则会出现double free的情况
            // 因为原本left是有值的，delete之后值还没有被置为nullptr
            // 由此可见，循环链表的删除则需要一个判断，是否为尾节点（下一个是头结点）
        }

        void red_black_tree::left_rotate(red_black_tree_node *const pivot) {
            red_black_tree_node *right = pivot->right;
            pivot->right = right->left;
            if (right->left != nullptr) {
                right->left->parent = pivot;
            }
            right->parent = pivot->parent;
            if (pivot->parent == nullptr) {
                root = right;
            } else if (parent_node(pivot)->left == pivot) {
                parent_node(pivot)->left = right;
            } else {
                parent_node(pivot)->right = right;
            }
            right->left = pivot;
            pivot->parent = right;
        }

        void red_black_tree::right_rotate(red_black_tree_node *const pivot) {
            red_black_tree_node *left = pivot->left;
            pivot->left = left->right;
            if (left->right != nullptr) {
                left->right->parent = pivot;
            }
            left->parent = pivot->parent;
            if (pivot->parent == nullptr) {
                root = left;
            } else if (parent_node(pivot)->left == pivot) {
                parent_node(pivot)->left = left;
            } else {
                parent_node(pivot)->right = left;
            }
            left->right = pivot;
            pivot->parent = left;
        }

        red_black_tree::red_black_tree_node *red_black_tree::parent_node(red_black_tree_node *const child) const {
            root; // NOLINT
            if (child == nullptr) {
                return nullptr;
            }
            auto *parent = (red_black_tree_node *) child->parent;
            return parent;
        }

        void red_black_tree::fix_after_append(red_black_tree_node *node) {
            for (; node != nullptr && root != node && parent_node(node)->color == RED;) {
                if (parent_node(node) == parent_node(parent_node(node))->left) {
                    red_black_tree_node *uncle = parent_node(parent_node(node))->right;
                    if (uncle != nullptr && uncle->color == RED) {
                        uncle->color = BLACK;
                        parent_node(node)->color = BLACK;
                        node = parent_node(parent_node(node));
                        node->color = RED;
                    } else {
                        if (node == parent_node(node)->right) {
                            node = parent_node(node);
                            left_rotate(node);
                        }
                        parent_node(node)->color = BLACK;
                        parent_node(parent_node(node))->color = RED;
                        right_rotate(parent_node(parent_node(node)));
                    }
                } else {
                    red_black_tree_node *uncle = parent_node(parent_node(node))->left;
                    if (uncle != nullptr && uncle->color == RED) {
                        uncle->color = BLACK;
                        parent_node(node)->color = BLACK;
                        node = parent_node(parent_node(node));
                        node->color = RED;
                    } else {
                        if (node == parent_node(node)->left) {
                            node = parent_node(node);
                            right_rotate(node);
                        }
                        parent_node(node)->color = BLACK;
                        parent_node(parent_node(node))->color = RED;
                        left_rotate(parent_node(parent_node(node)));
                    }
                }
            }
            root->color = BLACK;
        }

        red_black_tree::red_black_tree_node *red_black_tree::found(int value) {
            red_black_tree_node *current = root;
            for (; current != nullptr;) {
                if (current->value > value) {
                    current = current->left;
                } else if (current->value < value) {
                    current = current->right;
                } else {
                    break;
                }
            }
            return current;
        }

        red_black_tree::red_black_tree_node *red_black_tree::successor(red_black_tree_node *node) const {
            root; // NOLINT
            red_black_tree_node *most_left = node->right;
            for (; most_left != nullptr && most_left->left != nullptr;) {
                most_left = most_left->left;
            }
            return most_left;
        }

        void red_black_tree::fix_after_subtract(red_black_tree_node *node) {
            for (; node != root && BLACK == node->color;) {
                if (node == parent_node(node)->left) {
                    red_black_tree_node *sister = parent_node(node)->right;
                    if (RED == sister->color) {
                        sister->color = BLACK;
                        parent_node(node)->color = RED;
                        left_rotate(parent_node(node));
                        sister = parent_node(node)->right;
                    }
                    if ((sister->left == nullptr || BLACK == sister->left->color) &&
                        (sister->right == nullptr || BLACK == sister->right->color)) {
                        sister->color = RED;
                        node = parent_node(node);
                    } else {
                        if (sister->right == nullptr || BLACK == sister->right->color) {
                            sister->left->color = BLACK;
                            sister->color = RED;
                            right_rotate(sister);
                            sister = parent_node(node)->right;
                        }
                        sister->color = parent_node(node)->color;
                        parent_node(node)->color = BLACK;
                        if (sister->right != nullptr)
                            sister->right->color = BLACK;
                        left_rotate(parent_node(node));
                        node = root;
                    }
                } else {
                    red_black_tree_node *sister = parent_node(node)->left;
                    if (RED == sister->color) {
                        sister->color = BLACK;
                        parent_node(node)->color = RED;
                        right_rotate(parent_node(node));
                        sister = parent_node(node)->left;
                    }
                    if ((sister->left == nullptr || BLACK == sister->left->color) &&
                        (sister->right == nullptr || BLACK == sister->right->color)) {
                        sister->color = RED;
                        node = parent_node(node);
                    } else {
                        if (sister->left == nullptr || BLACK == sister->left->color) {
                            sister->right->color = BLACK;
                            sister->color = RED;
                            left_rotate(sister);
                            sister = parent_node(node)->left;
                        }
                        sister->color = parent_node(node)->color;
                        if (sister->left != nullptr)
                            sister->left->color = BLACK;
                        parent_node(node)->color = BLACK;
                        right_rotate(parent_node(node));
                        node = root;
                    }
                }
            }
            node->color = BLACK;
        }

        inline void red_black_tree::pre_order(unsigned int index, const unsigned int offset, red_black_tree_node *node,
                                              const unsigned int level, str::system_stream_io *out) const {
            if (node == nullptr) { return; }
            // 树放入数组中，由两个指标表示下标
            // 一个是父节点的下标，一个是父节点的左子（1）还是右子（2）
            // 既，根节点默认0，0
            // 这样父节点的下标乘以2，再加上一个数，即为本节点在数组中的下标
            index = index * 2 + offset;
            out->start(8u);
            out->append("[%4u]{level:%2u,\tcolor:%s\t,value:%d}",
                        index, level, node->color ? "Black\0" : "Red\0", node->value);
            out->end();
            pre_order(index, 1, node->left, level + 1, out);
            pre_order(index, 2, node->right, level + 1, out);
        }

        red_black_tree::red_black_tree() {
            root = nullptr;
        }

        red_black_tree::~red_black_tree() {
            delete root;
        }

        void red_black_tree::append(const int value) {
            auto *node = new red_black_tree_node(value);
            if (root == nullptr) {
                node->color = BLACK;
                root = node;
            }

            red_black_tree_node *current = root;
            red_black_tree_node *temp = current;
            for (; current != nullptr;) {
                temp = current;
                if (current->value > value) {
                    current = current->left;
                } else if (current->value < value) {
                    current = current->right;
                } else {
                    return;
                }
            }

            if (temp->value > value) {
                temp->left = node;
            } else {
                temp->right = node;
            }
            node->parent = temp;
            fix_after_append(node);
        }

        int red_black_tree::subtract(const int value) {
            red_black_tree_node *node = found(value);
            if (node == nullptr) { return 0; }
            if (node->left != nullptr && node->right != nullptr) {
                red_black_tree_node *temp = successor(node);
                node->value = temp->value;
                node = temp;
            }
            red_black_tree_node *replacement = node->left == nullptr ? node->right : node->left;
            if (replacement == nullptr) {
                if (node->parent == nullptr) {
                    root = nullptr;
                } else {
                    if (BLACK == node->color) {
                        fix_after_subtract(node);
                    }
                    if (node == parent_node(node)->left) {
                        parent_node(node)->left = nullptr;
                    } else {
                        parent_node(node)->right = nullptr;
                    }
                    node->parent = nullptr;
                }
            } else {
                replacement->color = BLACK;
                replacement->parent = parent_node(node);
                if (node->parent == nullptr) {
                    root = replacement;
                } else if (node == parent_node(node)->left) {
                    parent_node(node)->left = replacement;
                } else {
                    parent_node(node)->right = replacement;
                }
                node->parent = nullptr;
                node->left = nullptr;
                node->right = nullptr;
            }

            int result = node->value;
            delete node;
            return result;
        }

        void red_black_tree::scanning() const {
            auto *out = new str::system_stream_io(15u);
            pre_order(0u, 0, root, 1u, out);
            out->maybe(1u << 2u);
            out->append("输出的部分数据被缓冲了，直接释放内存，则不会显示ほんとうにすみませんでした");
            out->end();
        }
    }

    //main函数写法，既示例
    int virtual_cpu_run_main() {
        auto* time = new cpu::time_stamp();
        auto* counter = new cpu::counter_step();
        //第一步
        {
            //加载函数
            auto* out = new str::system_stream_io(1u);
            if (counter->read("add", 0, "../kuu/cpu_add.kuu") == 0) {
                out->maybe(1u);
                out->append("加载函数add时内存が足りないのかもしれません");
            }
            if (counter->read("sub", 0, "../kuu/cpu_sub.kuu") == 0) {
                out->maybe(1u);
                out->append("加载函数seb时内存が足りないのかもしれません");
            }
            if (counter->read("gather", 0, "../kuu/cpu_gather.kuu") == 0) {
                out->maybe(1u);
                out->append("加载函数gather时内存が足りないのかもしれません");
            }
            if (counter->read("test", 0, "../kuu/test_array_spacing.kuu") == 0) {
                out->maybe(1u);
                out->append("加载函数test时内存が足りないのかもしれません");
            }
            if (counter->read("main", 1, "cal 0 test") == 0) {
                out->maybe(1u);
                out->append("加载函数main时内存が足りないのかもしれません");
            }
            out->end();
        }
        //第二步
        for (;;) {
            if (counter->step_by_step() == 0) { break; }
        }
        //第三步
        counter->result();
        //结束
        //测试
        //第壹步
        if (counter->interlude() == 0) {
            auto* out = new str::system_stream_io(1u << 1u, 1u << 1u);
            out->append("気をつけて清理加载的函数内存に異常が出る");
            out->end();
            delete out;
        }
        //第贰步
        if (counter->read("main", 8, "mov ax 10000\nmov bx ax\ndec bx\njnz bx 1\ndec ax\njnz ax 4\nmov cx ax\nmov dx bx") == 0) {
            auto* out = new str::system_stream_io(1u, 1u);
            out->append("加载函数main时内存が足りないのかもしれません");
            out->end();
            delete out;
        }
        //第弎步
        for (;;) {
            if (counter->step_by_step() == 0) { break; }
        }
        //第肆步
        counter->result();
        //结束

        //int a[] = {50,12,18,19,35,65,45,84,64,68,74,10,100,59};
        int a[] = {50,35,78,27,45,56,90,40,48};
        auto* test = new rbt::red_black_tree();
        for (int& i:a) {
            test->append(i);
            test->scanning();
        }

        test->subtract(50);
        test->scanning();

        {
            auto *out = new str::system_stream_io(1u, 1u);
            unsigned long stamp = time->time_line_passed();
            unsigned long stamp_ms = stamp % 1000000000 / 1000000;
            unsigned long stamp_us = stamp % 1000000 / 1000;
            unsigned long stamp_ns = stamp % 1000;
            out->append("%llu[%d s %d ms %d us %d ns]", stamp, stamp / 1000000000, stamp_ms, stamp_us, stamp_ns);
            out->end();
            delete out;
        }

        delete test;
        delete counter;
        delete time;
        return 0;
    }
}

#endif //VIRTUAL_CPU_H
