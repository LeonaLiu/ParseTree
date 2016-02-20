/*
 * c++练习-4
 * 表达式树
 * 练习OOP
 * created by Yang Liu
 * 2016.02.19
 * version: 1.0
 */
 
#ifndef PARSETREE_H
#define PARSETREE_H

#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <memory>

namespace ParseTree
{
    class Node
    {
    public:
        virtual ~Node(){};
        virtual double calculate() const = 0;
    };
    
    //数字节点
    class NumberNode final : public Node
    {
    public:
        NumberNode(double n):num(n){};
        double calculate() const {return num;};
        
    private:
        double num;
    };
    
    //一元操作符
    class UnaryNode final : public Node
    {
    public:
        enum OpType {POST, NEG};
        UnaryNode(std::shared_ptr<Node> p, OpType op_type)
            :node(p),op(op_type){};
        double calculate() const;

    private:
        std::shared_ptr<Node> node;
        OpType op;
    };
    
    double UnaryNode::calculate() const
    {
        try
        {
            switch (op)
            {
                case POST:
                    return node->calculate();
                case NEG:
                    return -node->calculate();
                default :
                    throw std::runtime_error("UnaryNode: OpType Error.");
            }
        }
        catch (std::runtime_error &err)
        {
            std::cout << err.what() << std::endl << "Plz try again." << std::endl;
            exit(1);
        }
    }
   
   //二元操作符
    class BinaryNode final : public Node
    {
    public:
        enum OpType {ADD, MINUS, MUTIL, DIV}; //运算类型
        
        BinaryNode(std::shared_ptr<Node> node_l, std::shared_ptr<Node> node_r, OpType op_type)
            :node_left(node_l),node_right(node_r),op(op_type){};
        double calculate() const ;
        
    private:
        std::shared_ptr<Node> node_left;
        std::shared_ptr<Node> node_right;
        OpType op;
    };
    
    double BinaryNode::calculate() const
    {
        try
        {
            switch (op)
            {
                case ADD:
                    return node_left->calculate() + node_right->calculate();
                case MINUS:
                    return node_left->calculate() - node_right->calculate();
                case MUTIL:
                    return node_left->calculate() * node_right->calculate();
                case DIV:
                {
                    auto n = node_right->calculate();
                    if(n==0)
                    {
                        //除数=0时抛出异常
                        throw std::runtime_error("BinaryNode: divide by zero.");
                    }
                    return node_left->calculate() / n;
                }
                default :
                    throw std::runtime_error("BinaryNode: OpType Error.");
            }
        }
        catch (std::runtime_error &err)
        {
            std::cout << err.what() << std::endl << "Plz try again." << std::endl;
            exit(1);
        }
    }
    
    using std::string;
    using std::vector;

    //表达式树
    class ParseTree
    {
    public:
        //消除原始表达式中的所有空格
        ParseTree(const std::string& str);
        double calculate();
        
    private:
        typedef string::size_type size_type;
        
        //原始表达式
        std::string org_exp;
        //标记位置，用于分解原始表达式
        size_type pos;
        size_type end;
        //存储逆波兰表达式
        vector<std::string> RPN_exp;
        //表达式树根节点
        std::shared_ptr<Node> root;
        
        bool hasToken() const { return pos != end; };
        std::string nextToken(); //从原始表达式取一个运算符号或数
        size_type priority(const std::string& str) const;//判断运算符优先级
        void toRPN();  //将原始表达式转换为逆波兰表达式

        void buildParseTree(); //构建表达式树
        void doBuild(vector<std::shared_ptr<Node>>& vec, const string& op);
    };
    
    ParseTree::ParseTree(const string& str)
    {
        if(str.empty())
        {
            org_exp = str;
        }
        else
        {
            for (auto ch : str)
            {
                if (ch != ' ')
                {
                    org_exp += ch;
                }
            }
        }
        pos = 0;
        end = org_exp.length();
    }
    
    ParseTree::size_type ParseTree::priority(const std::string& str) const
    {
        switch(str[0])
        {
            case '(': case ')':
                return 1;
            case '+': case '-':
                return 2;
            case '*': case '/':
                return 3;
            case 'n': case 'p':
            {
                if(str=="neg" || str=="post")
                {
                    return 4;
                }
                //如果不是以上两个标记，向下执行default
            }
            default:
                throw std::runtime_error("ParseTree: Illegal expression.");
        }
    }

    string ParseTree::nextToken()
    {
        auto bp = pos;
        if(isdigit(org_exp[pos]))
        {
            while(pos!=end && isdigit(org_exp[++pos]));//整数部分
            if(pos!=end && org_exp[pos]=='.')
            {   
                //小数部分
                while(pos!=end && isdigit(org_exp[++pos]));
            }    
        }
        else //运算符号
        {
            //可能是一元运算符
            if (org_exp[pos] == '+' || org_exp[pos] == '-')
            {

                if (pos + 1 == end)
                {
                    throw std::runtime_error("ParseTree: Illegal expression.");
                }
                if (pos==0 || (org_exp[pos-1]!=')' && !isdigit(org_exp[pos-1])))
                {
                    //返回特殊标记，用于计算时与二元运算符区分
                    return (org_exp[pos++] == '+' ? "post" : "neg");
                }
            }
            ++pos; //其他运算符
        }
        return org_exp.substr(bp,pos-bp);
    }
    
    void ParseTree::toRPN()
    {
        vector<string> stack; //临时存放运算符

        while (hasToken())
        {
            string str = nextToken();

            if (isdigit(str[0]))
            {
                //数字直接放入RPN
                RPN_exp.push_back(str);
            }
            else if (str == "(" || stack.empty())
            {
                //( 或空栈，入栈
                stack.push_back(str);
            }
            else if (str == ")")
            {
                //弹出之前存储的符号，放入RPN中
                while (!stack.empty() && stack.back() != "(")
                {
                    RPN_exp.push_back(stack.back());
                    stack.pop_back();
                }
                if (stack.empty())
                {
                    //括号不匹配
                    throw std::runtime_error("ParseTree: Mismatched Parentheses.");
                }
                stack.pop_back(); //弹出"("
            }
            else
            {
                //弹出运算符，放入RPN，直到当前运算符优先级>=栈顶的运算符优先级
                while (!stack.empty() && priority(str) < priority(stack.back()))
                {
                    RPN_exp.push_back(stack.back());
                    stack.pop_back();
                }
                stack.push_back(str);
            }
        }
        //把剩下的运算符放入RPN
        std::copy(stack.crbegin(), stack.crend(), back_inserter(RPN_exp));
    }
    
    void ParseTree::buildParseTree()
    {
        vector<std::shared_ptr<Node>> tmp;
        
        for (auto it = RPN_exp.cbegin();it != RPN_exp.cend();++it)
        {
            if(isdigit((*it)[0])) //数字，暂时压入栈中待用
            {
                std::istringstream iss(*it);
                double i;
                iss>>i;
                tmp.push_back(std::make_shared<NumberNode>(NumberNode(i)));
            }
            else
            {
                //运算符，自底向上构建表达式树
                doBuild(tmp, *it);
            }
        }
        
        root = tmp.back();//最后一个元素就是整个表达式树的根
        tmp.pop_back();
    }
    
    void ParseTree::doBuild(vector<std::shared_ptr<Node>>& vec, const string& op)
    {
        switch(op[0])
        {
            case '+':
            {
                auto p1 = vec.back();
                vec.pop_back();
                auto p2 = vec.back();
                vec.pop_back();
                vec.push_back(std::make_shared<BinaryNode>(BinaryNode(p2, p1, BinaryNode::ADD)));
                return;
            }
            case '-':
            {
                auto p1 = vec.back();
                vec.pop_back();
                auto p2 = vec.back();
                vec.pop_back();
                vec.push_back(std::make_shared<BinaryNode>(BinaryNode(p2, p1, BinaryNode::MINUS)));
                return;
            }
            case '*':
            {
                auto p1 = vec.back();
                vec.pop_back();
                auto p2 = vec.back();
                vec.pop_back();
                vec.push_back(std::make_shared<BinaryNode>(BinaryNode(p2, p1, BinaryNode::MUTIL)));
                return;
            }
            case '/':
            {
                auto p1 = vec.back();
                vec.pop_back();
                auto p2 = vec.back();
                vec.pop_back();
                vec.push_back(std::make_shared<BinaryNode>(BinaryNode(p2, p1, BinaryNode::DIV)));
                return;
            }
            case 'p'://取正
            {
                auto p = vec.back();
                vec.pop_back();
                vec.push_back(std::make_shared<UnaryNode>(UnaryNode(p, UnaryNode::POST)));
                return;
            }
            case 'n'://取负
            {
                auto p = vec.back();
                vec.pop_back();
                vec.push_back(std::make_shared<UnaryNode>(UnaryNode(p, UnaryNode::NEG)));
                return;
            }
            default:
                throw std::runtime_error("ParseTree: Illegal expression.");
        }
    }
    
    double ParseTree::calculate()
    {
        if(org_exp.empty())
        {
            return 0;
        }
        try
        {
            toRPN();
            buildParseTree();
            return root->calculate();
        }
        catch (std::runtime_error &err)
        {
            std::cout << err.what() << std::endl << "Plz try again." << std::endl;
            exit(1);
        }
    }
}
#endif
