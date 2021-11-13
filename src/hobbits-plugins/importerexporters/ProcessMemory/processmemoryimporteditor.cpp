#include "processmemoryimporteditor.h"
#include "ui_processmemoryimporteditor.h"
#include "Windows.h"
#include <windows.h>
#include "WinUser.h"
#include <QMetaType>
#include <stdio.h>
#include <psapi.h>

ProcessMemoryImportEditor::ProcessMemoryImportEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::ProcessMemoryImportEditor()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);
    ui->plainTextEdit->setReadOnly(true);
    ui->Memory_debug->setReadOnly(true);
    ui->buffer_sb->setRange(1, 100000);
    m_paramHelper->addTextEditStringParameter("Process_Info_Import", ui->plainTextEdit);
    m_paramHelper->addTextEditStringParameter("Process_Memory_Import", ui->Memory_debug);
    m_paramHelper->addSpinBoxIntParameter("Buffer", ui->buffer_sb);
    EnumWindows(StaticEnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

ProcessMemoryImportEditor::~ProcessMemoryImportEditor()
{
    delete ui;
}

QString ProcessMemoryImportEditor::title()
{
    return "Configure Process Memory";
}

Parameters ProcessMemoryImportEditor::parameters()
{
    /* Setting the member vaules to paramaters*/
    auto params = m_paramHelper->getParametersFromUi();
    params.insert("Windows_title", m_windows_title);
    params.insert("Process_ID", (qint64)m_id_paramater);
    params.insert("Process_Handle", (qint64)m_HANDLE_paramter);
    return params;
}

bool ProcessMemoryImportEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void ProcessMemoryImportEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{

}

void ProcessMemoryImportEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{

}

void ProcessMemoryImportEditor::on_refresh_btn_pressed()
{
    /* This refreshes the listWidget, the process info display,
    and the memory addresses box */
    ui->listWidget->clear();
    ui->plainTextEdit->clear();
    EnumWindows(StaticEnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

void ProcessMemoryImportEditor::on_listWidget_itemClicked()
{
    /* converting the handle of the currently clikced item
    and turning it back into a HWND (the item contains quin64) */
    ui->plainTextEdit->clear();
    ui->Memory_debug->clear();
    QVariant Qvar = ui->listWidget->currentItem()->data(Qt::UserRole);
    quint64 handle_convert = Qvar.toULongLong();
    HWND param_handle = (HWND)handle_convert;

    //ui->plainTextEdit->setPlainText(QString::number((quint64)param_handle));
    ui->plainTextEdit->insertPlainText(QString("Window Title: %1\n").arg(ui->listWidget->currentItem()->text()));
    m_windows_title = ui->listWidget->currentItem()->text();

    /* After the handle is converted back into a HWND, the 
    value is used in the call PrintMemoryInfo */
    ProcessMemoryImportEditor::PrintMemoryInfo(param_handle);
}

void ProcessMemoryImportEditor::PrintMemoryInfo(HWND hwnd)
{
    /* This function display the PID, process handle, windows
    handle, memory information, and memory locations */

    PROCESS_MEMORY_COUNTERS pmc;

    // Print the process identifier.
    DWORD id;

    //Finding the PID from the windows handle
    if(!GetWindowThreadProcessId(hwnd, &id))
    {
        DWORD error = GetLastError();
        ui->plainTextEdit->insertPlainText(QString("Error Getting Window Thread Process ID: %1 ").arg(error));
        return;
    }

    // Print information about the memory usage of the process.
    ui->plainTextEdit->insertPlainText(QString("Process ID: %1 \n").arg((quint64)id));
    ui->plainTextEdit->insertPlainText(QString("Windows Handle: %1 \n").arg((quint64)hwnd));

    //Opening the Handle to acess the information
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, id);

    ProcessMemoryImportEditor::MemoryDumpImport(id, process);

    //This is just setting a global var to the current handle to add to a paramater 
    m_HANDLE_paramter = process;
    m_id_paramater = id;
    m_HWND_paramater = hwnd;

    //Current Process Handle
    ui->plainTextEdit->insertPlainText(QString("Process Handle: %1 \n").arg((quint64)process));

    //If process == NULL, then error opening up the process
    if (NULL == process)
    {
        DWORD error = GetLastError();
        ui->plainTextEdit->insertPlainText(QString("\nError Opening Process: %1\n").arg(error));
        return;
    }

    if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc)))
    {
        ui->plainTextEdit->insertPlainText(QString("\tPageFaultCount: 0x%08X\n").arg(pmc.PageFaultCount));
        ui->plainTextEdit->insertPlainText(QString("\tPeakWorkingSetSize: 0x%08X\n").arg(pmc.PeakWorkingSetSize));
        ui->plainTextEdit->insertPlainText(QString("\tWorkingSetSize: 0x%08X\n").arg(pmc.WorkingSetSize));
        ui->plainTextEdit->insertPlainText(QString("\tQuotaPeakPagedPoolUsage: 0x%08X\n").arg(pmc.QuotaPeakPagedPoolUsage));
        ui->plainTextEdit->insertPlainText(QString("\tQuotaPagedPoolUsage: 0x%08X\n").arg(pmc.QuotaPagedPoolUsage));
        ui->plainTextEdit->insertPlainText(QString("\tQuotaPeakNonPagedPoolUsage: 0x%08X\n").arg(pmc.QuotaPeakNonPagedPoolUsage));
        ui->plainTextEdit->insertPlainText(QString("\tQuotaNonPagedPoolUsage: 0x%08X\n").arg(pmc.QuotaNonPagedPoolUsage));
        ui->plainTextEdit->insertPlainText(QString("\tPagefileUsage: 0x%08X\n").arg(pmc.PagefileUsage)); 
        ui->plainTextEdit->insertPlainText(QString("\tPeakPagefileUsage: 0x%08X\n").arg(pmc.PeakPagefileUsage));            
    }
    //If error with GetProcessMemoryInfo, then display error getting memory info
    else
    {
        DWORD error = GetLastError();
        ui->plainTextEdit->insertPlainText(QString("Error Getting Process Memory Information: %1").arg(error));
    }
}

void ProcessMemoryImportEditor::on_SearchBox_textChanged()
{
    /* FInds items with text starting with the characters typed
    in the textBox */
    QString searchText = ui->SearchBox->text();
    int listWidgetSize = ui->listWidget->count();
 
    for (int k1 = 0; k1 < listWidgetSize; k1++)
    {
        if (ui->listWidget->item(k1)->text().startsWith(searchText))
        {
            ui->listWidget->item(k1)->setHidden(false);
        }
        else
        {
            ui->listWidget->item(k1)->setHidden(true);
        }
    }
}

void ProcessMemoryImportEditor::MemoryDumpImport(DWORD procID, HANDLE process)
{
    ui->Memory_debug->clear();

    unsigned char* addr = 0;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

    MEMORY_BASIC_INFORMATION mbi;

    DWORD address = 0;

    char membuffer[8192];

    SIZE_T bytesRead;

    while (VirtualQueryEx(hProc, addr, &mbi, sizeof(mbi)))
    {
        if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && mbi.Protect != PAGE_GUARD)
        {
            ui->Memory_debug->insertPlainText(QString("\tBase : 0x%1\n").arg((uintptr_t)mbi.BaseAddress));
            SIZE_T bytesToRead = qMin((SIZE_T)8192, mbi.RegionSize);
            ReadProcessMemory(process, (LPCVOID)mbi.BaseAddress, membuffer, bytesToRead, &bytesRead);
            ui->Memory_debug->insertPlainText(QString("\tEnd : 0x%1\n").arg((uintptr_t)mbi.BaseAddress + mbi.RegionSize));
        }
        else
        {
            ui->Memory_debug->insertPlainText("\t.....Error Getting Memory Address.....\n");
        }
        addr += mbi.RegionSize;
    }

}

BOOL ProcessMemoryImportEditor::StaticEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    //Finding the handle and returning that to EnumWindowsProc
    ProcessMemoryImportEditor *pThis = reinterpret_cast<ProcessMemoryImportEditor*>(lParam);
    return pThis->EnumWindowsProc(hwnd);
}

BOOL ProcessMemoryImportEditor::EnumWindowsProc(HWND hwnd)
{
    /*Finds all the windows running on the machine and displays them
    in a listWidget. It also adds the handle to each of the widget 
    so that the data can be accessed later
    */
    WCHAR titleProc[255];

    if(GetWindowTextW(hwnd, titleProc, 255))
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(QString::fromWCharArray(titleProc));
        item->setData(Qt::UserRole, (quint64)hwnd);
        ui->listWidget->addItem(item);
    }
    
    return true;
}