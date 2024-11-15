#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

namespace edge_infra {
namespace infra_controller {

// 事件类型定义
enum class EventType {
    SYSTEM_START,
    SYSTEM_STOP,
    SERVICE_REGISTER,
    SERVICE_UNREGISTER,
    MESSAGE_RECEIVED,
    CONNECTION_ESTABLISHED,
    CONNECTION_LOST,
    ERROR_OCCURRED,
    CUSTOM
};

// 事件数据结构
struct Event {
    EventType type;
    std::string source;
    std::string target;
    std::unordered_map<std::string, std::string> data;
    uint64_t timestamp;
    uint32_t priority;
    
    Event(EventType t = EventType::CUSTOM, const std::string& src = "", 
          const std::string& tgt = "");
    
    void setData(const std::string& key, const std::string& value);
    std::string getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
};

// 事件处理器接口
class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual bool handleEvent(const Event& event) = 0;
    virtual std::string getHandlerName() const = 0;
    virtual std::vector<EventType> getSupportedEvents() const = 0;
};

// 工作流步骤
class WorkflowStep {
public:
    enum class StepType {
        CONDITION,
        ACTION,
        PARALLEL,
        SEQUENTIAL
    };
    
    enum class StepStatus {
        PENDING,
        RUNNING,
        COMPLETED,
        FAILED,
        SKIPPED
    };
    
private:
    std::string name_;
    StepType type_;
    std::atomic<StepStatus> status_;
    std::function<bool(const Event&)> condition_;
    std::function<bool(const Event&)> action_;
    std::vector<std::shared_ptr<WorkflowStep>> children_;
    
public:
    WorkflowStep(const std::string& name, StepType type);
    virtual ~WorkflowStep() = default;
    
    // 步骤配置
    void setCondition(std::function<bool(const Event&)> cond);
    void setAction(std::function<bool(const Event&)> act);
    void addChild(std::shared_ptr<WorkflowStep> child);
    
    // 执行控制
    virtual bool execute(const Event& event);
    virtual void reset();
    
    // 状态查询
    const std::string& getName() const { return name_; }
    StepType getType() const { return type_; }
    StepStatus getStatus() const { return status_.load(); }
    
    // 调试功能
    std::string statusToString() const;
    void printStepTree(int indent = 0) const;
    
protected:
    void setStatus(StepStatus status) { status_.store(status); }
    bool executeChildren(const Event& event);
    bool executeChildrenParallel(const Event& event);
    bool executeChildrenSequential(const Event& event);
};

// StackFlow主控制器
class StackFlow {
private:
    std::string name_;
    bool running_;
    std::atomic<bool> stop_requested_;
    
    // 事件处理
    std::unordered_map<EventType, std::vector<std::shared_ptr<EventHandler>>> handlers_;
    std::queue<Event> event_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::thread event_thread_;
    
    // 工作流管理
    std::unordered_map<std::string, std::shared_ptr<WorkflowStep>> workflows_;
    std::mutex workflow_mutex_;
    
    // 统计信息
    std::atomic<uint64_t> events_processed_;
    std::atomic<uint64_t> workflows_executed_;
    std::atomic<uint64_t> errors_count_;
    
public:
    explicit StackFlow(const std::string& name);
    ~StackFlow();
    
    // 禁用拷贝
    StackFlow(const StackFlow&) = delete;
    StackFlow& operator=(const StackFlow&) = delete;
    
    // 生命周期管理
    bool start();
    void stop();
    bool isRunning() const { return running_; }
    
    // 事件处理器管理
    void registerHandler(EventType type, std::shared_ptr<EventHandler> handler);
    void unregisterHandler(EventType type, const std::string& handler_name);
    void unregisterAllHandlers(EventType type);
    
    // 事件发布
    void publishEvent(const Event& event);
    void publishEvent(EventType type, const std::string& source = "", 
                     const std::string& target = "");
    
    // 工作流管理
    void registerWorkflow(const std::string& name, std::shared_ptr<WorkflowStep> workflow);
    void unregisterWorkflow(const std::string& name);
    bool executeWorkflow(const std::string& name, const Event& trigger_event);
    
    // 统计信息
    uint64_t getEventsProcessed() const { return events_processed_.load(); }
    uint64_t getWorkflowsExecuted() const { return workflows_executed_.load(); }
    uint64_t getErrorsCount() const { return errors_count_.load(); }
    size_t getQueueSize() const;
    
    // 调试功能
    void enableDebug(bool enable = true);
    void printStatistics() const;
    void printRegisteredHandlers() const;
    void printRegisteredWorkflows() const;
    
    const std::string& getName() const { return name_; }
    
private:
    void eventProcessingLoop();
    void processEvent(const Event& event);
    void triggerWorkflows(const Event& event);
    
    bool debug_enabled_;
    void debugLog(const std::string& message) const;
};

// 便捷的事件处理器基类
class SimpleEventHandler : public EventHandler {
private:
    std::string name_;
    std::vector<EventType> supported_events_;
    std::function<bool(const Event&)> handler_func_;
    
public:
    SimpleEventHandler(const std::string& name, 
                      const std::vector<EventType>& events,
                      std::function<bool(const Event&)> func);
    
    bool handleEvent(const Event& event) override;
    std::string getHandlerName() const override { return name_; }
    std::vector<EventType> getSupportedEvents() const override { return supported_events_; }
};

// 工具函数
std::string eventTypeToString(EventType type);
EventType stringToEventType(const std::string& str);

} // namespace infra_controller
} // namespace edge_infra
