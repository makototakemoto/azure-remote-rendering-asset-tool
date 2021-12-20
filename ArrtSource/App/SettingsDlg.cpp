#include "SettingsDlg.h"
#include <Storage/StorageAccount.h>
#include <QCoreApplication>
#include <QMessageBox>
#include <Rendering/ArrAccount.h>

SettingsDlg::SettingsDlg(StorageAccount* storage, ArrAccount* arrClient, QWidget* parent)
    : QDialog(parent)
{
    m_storageAccount = storage;
    m_arrClient = arrClient;

    setupUi(this);

    // Storage Account
    {
        StorageName->setText(m_storageAccount->GetAccountName());
        StorageKey->setText(m_storageAccount->GetAccountKey());
        StorageEndpoint->setText(m_storageAccount->GetEndpointUrl());
    }

    // ARR Account
    {
        ArrAccountID->setText(m_arrClient->GetAccountId());
        ArrAccountKey->setText(m_arrClient->GetAccountKey());

        // fill domain combobox
        {
            std::vector<ArrAccountDomainInfo> domains;
            m_arrClient->GetAvailableAccountDomains(domains);

            int idx = -1;
            for (int i = 0; i < domains.size(); ++i)
            {
                const auto& domain = domains[i];
                ArrAccountDomain->addItem(domain.m_name, domain.m_url);

                if (m_arrClient->GetAccountDomain() == domain.m_url)
                {
                    idx = i;
                }
            }

            ArrAccountDomain->setCurrentIndex(idx);
        }

        // fill region combobox
        {
            std::vector<ArrRegionInfo> regions;
            m_arrClient->GetAvailableRegions(regions);

            int idx = -1;
            for (int i = 0; i < regions.size(); ++i)
            {
                const auto& region = regions[i];
                ArrRegion->addItem(region.m_name, region.m_url);

                if (m_arrClient->GetRegion() == region.m_url)
                {
                    idx = i;
                }
            }

            ArrRegion->setCurrentIndex(idx);
        }
    }
}

SettingsDlg::~SettingsDlg() = default;

void SettingsDlg::closeEvent(QCloseEvent*)
{
    ApplyArr();
    ApplyStorage();
}

void SettingsDlg::on_Buttons_rejected()
{
    closeEvent(nullptr);
    accept();
}

void SettingsDlg::on_TestArr_clicked()
{
    ApplyArr();

    for (int i = 0; i < 100; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCoreApplication::processEvents();

        switch (m_arrClient->GetConnectionStatus())
        {
            case AccountConnectionStatus::Authenticated:
                QMessageBox::information(this, "Test ARR account connect", "Connection to ARR account succeeded.", QMessageBox::Ok);
                return;

            case AccountConnectionStatus::NotAuthenticated:
                QMessageBox::warning(this, "Test ARR account connect", "User authentication failed.", QMessageBox::Ok);
                return;

            case AccountConnectionStatus::InvalidCredentials:
                QMessageBox::warning(this, "Test ARR account connect", "The provided credentials are invalid.", QMessageBox::Ok);
                return;

            case AccountConnectionStatus::CheckingCredentials:
                break;
        }
    }

    QMessageBox::warning(this, "Test ARR account connect", "Connection timed out - credentials are most likely incorrect.", QMessageBox::Ok);
}

void SettingsDlg::on_TestStorage_clicked()
{
    ApplyStorage();

    for (int i = 0; i < 100; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCoreApplication::processEvents();

        switch (m_storageAccount->GetConnectionStatus())
        {
            case AccountConnectionStatus::Authenticated:
                QMessageBox::information(this, "Test storage account connection", "Connection to storage account succeeded.", QMessageBox::Ok);
                return;

            case AccountConnectionStatus::NotAuthenticated:
                QMessageBox::warning(this, "Test storage account connection", "User authentication failed.", QMessageBox::Ok);
                return;

            case AccountConnectionStatus::InvalidCredentials:
                QMessageBox::warning(this, "Test storage account connection", "The provided credentials are invalid.", QMessageBox::Ok);
                return;

            case AccountConnectionStatus::CheckingCredentials:
                break;
        }
    }

    QMessageBox::warning(this, "Test storage account connection", "Connection timed out - credentials are most likely incorrect.", QMessageBox::Ok);
}

void SettingsDlg::ApplyArr()
{
    m_arrClient->SetSettings(ArrAccountID->text(), ArrAccountKey->text(), ArrAccountDomain->currentData().toString(), ArrRegion->currentData().toString());

    m_arrClient->ConnectToArrAccount();
}

void SettingsDlg::ApplyStorage()
{
    m_storageAccount->SetSettings(StorageName->text(), StorageKey->text(), StorageEndpoint->text());
    m_storageAccount->ConnectToStorageAccount();
}