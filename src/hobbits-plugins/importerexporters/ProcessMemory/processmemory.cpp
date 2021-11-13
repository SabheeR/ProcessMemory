#include "processmemory.h"
#include "processmemoryimporteditor.h"
#include "processmemoryexporteditor.h"
#include "Windows.h"

ProcessMemory::ProcessMemory() 
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"Process_Info_Import", ParameterDelegate::ParameterType::String},
        {"Process_Memory_Import", ParameterDelegate::ParameterType::String},
        {"Windows_title", ParameterDelegate::ParameterType::String},
        {"Process_ID", ParameterDelegate::ParameterType::Integer},
        {"Process_Handle", ParameterDelegate::ParameterType::Integer},
        {"Buffer", ParameterDelegate::ParameterType::Integer}
    };
    
    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        QString windows_title = parameters.value("Windows_title").toString();
                        return QString("Windows Title: %1").arg(windows_title);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new ProcessMemoryImportEditor(delegate);
                    });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        
    };

    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Export").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new ProcessMemoryExportEditor(delegate);
                    });
}

ImporterExporterInterface* ProcessMemory::createDefaultImporterExporter()
{
    return new ProcessMemory();
}

QString ProcessMemory::name()
{
    return "Process Memory";
}

QString ProcessMemory::description()
{
    return "Takes a snapshot of an active task and displays all used memory locations";
}

QStringList ProcessMemory::tags()
{
    return {"Memory Adresses, Running Processes"};
}

bool ProcessMemory::canExport()
{
    return false;
}

bool ProcessMemory::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate>  ProcessMemory::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate>  ProcessMemory::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult>  ProcessMemory::importBits(const Parameters &parameters,
                                                       QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    int buffer_array_amount = parameters.value("Buffer").toInt();
    const int a = (const int)buffer_array_amount;

    auto proc_memory_bitarray = QSharedPointer<BitArray>(new BitArray);

    DWORD procID = parameters.value("Process_ID").toInt();

    unsigned char* addr = 0;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

    MEMORY_BASIC_INFORMATION mbi; DWORD address = 0; SIZE_T bytesRead; qint64 offset = 0;

    char membuffer[8192];

    const int i = 1;
    char membuf[i];
    
    /* This loop reads through all the processes memory locations (via 
    VirtualQueryEx) and finds to see if it is a valid location. If 
    not, it will display "error getting memory location" and in the 
    imported data, it will show as "...." */
    while (VirtualQueryEx(hProc, addr, &mbi, sizeof(mbi)))
    {
        if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && mbi.Protect != PAGE_GUARD)
        {
            SIZE_T bytesToRead = qMin((SIZE_T)buffer_array_amount, mbi.RegionSize);
            if (ReadProcessMemory(hProc, (LPCVOID)mbi.BaseAddress, membuffer, bytesToRead, &bytesRead) == 0)
            {
                //DWORD error = GetLastError();
                //return ImportResult::error(QString("Error, Failed to Read Process Memory: %1").arg(error));
            }
            else
            {
                proc_memory_bitarray->setBytes(offset, membuffer, 0, bytesRead);
                offset = offset + bytesRead;
            }
        }
        addr += mbi.RegionSize;
    }
    /*All of the memory locations are stored in the bit array and 
    is returned in a bit container*/
    auto proc_memory_bitContainer = BitContainer::create(proc_memory_bitarray);
    return ImportResult::result(proc_memory_bitContainer, parameters);

    //return ImportResult::error("Import not implemented");
}


QSharedPointer<ExportResult>  ProcessMemory::exportBits(QSharedPointer<const BitContainer> container,
                                                       const Parameters &parameters,
                                                       QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return ExportResult::error("Export not implemented");
}