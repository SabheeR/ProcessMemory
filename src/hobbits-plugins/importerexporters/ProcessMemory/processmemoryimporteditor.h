#ifndef PROCESSMEMORYIMPORTEDITOR_H
#define PROCESSMEMORYIMPORTEDITOR_H

#include "ui_processmemoryimporteditor.h"
#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include "Windows.h"

namespace Ui
{
class ProcessMemoryImportEditor;
}

class ProcessMemoryImportEditor : public AbstractParameterEditor
{
    Q_OBJECT
    HWND set_hwnd_param;
    HANDLE set_handle_param;
    DWORD set_id_param;
    QString set_memory_dump;

public:
    ProcessMemoryImportEditor(QSharedPointer<ParameterDelegate> delegate);
    ~ProcessMemoryImportEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

    //Public stuff for task manager list widget thing
    explicit ProcessMemoryImportEditor(QWidget *parent = 0);
    //~ProcessMemoryImportEditor();

    //EnumWindowsProc functions and variables
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);
    BOOL EnumWindowsProc(HWND hwnd);
    HWND m_HWND_paramater;
    HANDLE m_HANDLE_paramter;
    DWORD m_id_paramater;
    QString m_windows_title;

    QString set_memory_locations_parameter;

private slots:
    void on_refresh_btn_pressed();
    void on_listWidget_itemClicked();
    void on_SearchBox_textChanged();
    void PrintMemoryInfo(HWND hwnd);
    void MemoryDumpImport(DWORD procID, HANDLE process);

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;


    QSharedPointer<ParameterHelper> m_paramHelper;

private:
    Ui::ProcessMemoryImportEditor *ui;

};

#endif // PROCESSMEMORYIMPORTEDITOR_H