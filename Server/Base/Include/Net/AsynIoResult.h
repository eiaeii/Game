#ifndef ASYNIORESULT_H
#define ASYNIORESULT_H

#include "AsynIoFrame.h"
#include "NetDefine.h"

struct IAsynIoResult;
struct IAsynIoBuffer;

class AsynIoResult : public IAsynIoResult
{
public:

	AsynIoResult();

	AsynIoResult(AsynIoType nType, IAsynIoHandler * pHandler);

	void SetAsynIoHandler(IAsynIoHandler* pHandler) override;

	void Complete() override;

	void PrepareBuffer(unsigned long nLen) override;

	void SetAsynIoBuffer(IAsynIoBuffer* pBufferList, unsigned long nBufferCount) override;

	IAsynIoBuffer* GetAsynIoBufferList() override;

	unsigned long GetAsynIoBufferCount() const override;

	unsigned long GetTransferredBytes() const override;

	void  SetTransferredBytes(unsigned long bytes) override;

	bool IsSuccess() const override;

	unsigned long GetErrorCode() const override;

	void SetErrorCode(unsigned long nCode) override;

	IAsynIoDevice* GetAsynIoDevice() const override;

	void SetAsynIoDevice(IAsynIoDevice* pDevice) override;

	AsynIoType GetAsynIoType() const override;

	void SetAsynIoType(AsynIoType btType) override;

	bool IsSimulation() override;

	void SetSimulation(bool bSimulation) override;

	void Release() override;

protected:
	//////////////////////////////////////////////////////////////////////////
	IAsynIoHandler *m_pHandler = nullptr;
	IAsynIoDevice *m_pDevice = nullptr;
	AsynIoBuffer m_BuffList[MAX_ASYN_IO_BUFF_COUNT];
	unsigned long m_dwBuffCount = 0;
	unsigned long m_dwTransferredBytes = 0;
	unsigned long m_dwErrorCode = 0;
	AsynIoType m_Type = AsynIoType::AsynIoType_Unknow;
	bool m_bSimulation = false;
};

#endif //ASYNIORESULT_H
