#!/usr/bin/env python3
"""
订单流交易系统测试脚本
用于验证应用程序是否正常工作
"""

import subprocess
import sys
import time
import os

def test_app_startup():
    """测试应用程序启动"""
    print("🧪 测试应用程序启动...")
    
    # 检查可执行文件
    app_path = "build/tradeview_app"
    if not os.path.exists(app_path):
        print("❌ 错误: 可执行文件不存在")
        return False
    
    print(f"✅ 找到可执行文件: {app_path}")
    
    # 测试启动
    try:
        # 设置环境变量
        env = os.environ.copy()
        env['QT_QPA_PLATFORM'] = 'offscreen'
        env['QT_QUICK_BACKEND'] = 'software'
        
        # 启动应用程序
        process = subprocess.Popen(
            [app_path],
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        # 等待一段时间
        time.sleep(3)
        
        # 检查进程是否还在运行
        if process.poll() is None:
            print("✅ 应用程序成功启动并运行")
            process.terminate()
            process.wait()
            return True
        else:
            stdout, stderr = process.communicate()
            print(f"❌ 应用程序启动失败")
            print(f"标准输出: {stdout}")
            print(f"错误输出: {stderr}")
            return False
            
    except Exception as e:
        print(f"❌ 启动测试失败: {e}")
        return False

def test_build_system():
    """测试构建系统"""
    print("\n🔨 测试构建系统...")
    
    try:
        # 检查CMakeLists.txt文件
        cmake_files = [
            "CMakeLists.txt",
            "model/CMakeLists.txt", 
            "qml/CMakeLists.txt",
            "control/CMakeLists.txt"
        ]
        
        for cmake_file in cmake_files:
            if os.path.exists(cmake_file):
                print(f"✅ 找到: {cmake_file}")
            else:
                print(f"❌ 缺失: {cmake_file}")
                return False
        
        # 检查构建目录
        if os.path.exists("build"):
            print("✅ 构建目录存在")
        else:
            print("❌ 构建目录不存在")
            return False
        
        return True
        
    except Exception as e:
        print(f"❌ 构建系统测试失败: {e}")
        return False

def test_qml_files():
    """测试QML文件"""
    print("\n🎨 测试QML文件...")
    
    qml_files = [
        "qml/main.qml",
        "qml/qml.qrc",
        "qml/pages/CandlestickPage.qml",
        "qml/pages/FootprintPage.qml",
        "qml/pages/OrderBookPage.qml",
        "qml/pages/TickTradePage.qml",
        "qml/pages/MicroOrderBookPage.qml",
        "qml/pages/SettingsPage.qml",
        "qml/components/ChartToolbar.qml",
        "qml/components/StatusBar.qml",
        "qml/components/NavigationBar.qml",
        "qml/components/DataTable.qml",
        "qml/components/FootprintChart.qml"
    ]
    
    missing_files = []
    for qml_file in qml_files:
        if os.path.exists(qml_file):
            print(f"✅ 找到: {qml_file}")
        else:
            print(f"❌ 缺失: {qml_file}")
            missing_files.append(qml_file)
    
    if missing_files:
        print(f"❌ 缺失 {len(missing_files)} 个QML文件")
        return False
    
    print("✅ 所有QML文件都存在")
    return True

def test_cpp_files():
    """测试C++文件"""
    print("\n⚙️ 测试C++文件...")
    
    cpp_files = [
        "control/main.cpp",
        "control/application.h",
        "control/application.cpp",
        "control/datamanager.h",
        "control/datamanager.cpp",
        "control/uimanager.h",
        "control/uimanager.cpp",
        "model/candlestickmodel.h",
        "model/candlestickmodel.cpp",
        "model/ticktrademodel.h",
        "model/ticktrademodel.cpp",
        "model/orderbookmodel.h",
        "model/orderbookmodel.cpp",
        "model/footprintmodel.h",
        "model/footprintmodel.cpp",
        "model/microorderbookmodel.h",
        "model/microorderbookmodel.cpp"
    ]
    
    missing_files = []
    for cpp_file in cpp_files:
        if os.path.exists(cpp_file):
            print(f"✅ 找到: {cpp_file}")
        else:
            print(f"❌ 缺失: {cpp_file}")
            missing_files.append(cpp_file)
    
    if missing_files:
        print(f"❌ 缺失 {len(missing_files)} 个C++文件")
        return False
    
    print("✅ 所有C++文件都存在")
    return True

def main():
    """主测试函数"""
    print("🚀 订单流交易系统测试开始")
    print("=" * 50)
    
    tests = [
        ("构建系统", test_build_system),
        ("C++文件", test_cpp_files),
        ("QML文件", test_qml_files),
        ("应用程序启动", test_app_startup)
    ]
    
    results = []
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"❌ {test_name}测试异常: {e}")
            results.append((test_name, False))
    
    # 输出测试结果
    print("\n" + "=" * 50)
    print("📊 测试结果总结:")
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "✅ 通过" if result else "❌ 失败"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
    
    print(f"\n总计: {passed}/{total} 项测试通过")
    
    if passed == total:
        print("🎉 所有测试通过！应用程序运行正常")
        return 0
    else:
        print("⚠️ 部分测试失败，请检查相关问题")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 