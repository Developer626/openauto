/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#include <f1x/aasdk/USB/AOAPDevice.hpp>
#include <f1x/aasdk/Transport/SSLWrapper.hpp>
#include <f1x/aasdk/Transport/USBTransport.hpp>
#include <f1x/aasdk/Transport/TCPTransport.hpp>
#include <f1x/aasdk/Messenger/Cryptor.hpp>
#include <f1x/openauto/autoapp/Projection/AndroidAutoEntityFactory.hpp>
#include <f1x/openauto/autoapp/Projection/AndroidAutoEntity.hpp>

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace projection
{

AndroidAutoEntityFactory::AndroidAutoEntityFactory(aasdk::usb::IUSBWrapper& usbWrapper,
                                                   boost::asio::io_service& ioService,
                                                   configuration::IConfiguration::Pointer configuration,
                                                   IServiceFactory& serviceFactory)
    : usbWrapper_(usbWrapper)
    , ioService_(ioService)
    , configuration_(std::move(configuration))
    , serviceFactory_(serviceFactory)
{

}

IAndroidAutoEntity::Pointer AndroidAutoEntityFactory::create(aasdk::usb::DeviceHandle deviceHandle)
{
    auto aoapDevice(aasdk::usb::AOAPDevice::create(usbWrapper_, ioService_, deviceHandle));
    auto transport(std::make_shared<aasdk::transport::USBTransport>(ioService_, aoapDevice));

    return create(std::move(transport));
}

IAndroidAutoEntity::Pointer AndroidAutoEntityFactory::create(aasdk::tcp::ITCPEndpoint::Pointer tcpEndpoint)
{
    auto transport(std::make_shared<aasdk::transport::TCPTransport>(ioService_, std::move(tcpEndpoint)));
    return create(std::move(transport));
}

IAndroidAutoEntity::Pointer AndroidAutoEntityFactory::create(aasdk::transport::ITransport::Pointer transport)
{
    auto sslWrapper(std::make_shared<aasdk::transport::SSLWrapper>());
    auto cryptor(std::make_shared<aasdk::messenger::Cryptor>(std::move(sslWrapper)));

    return std::make_shared<AndroidAutoEntity>(ioService_, std::move(cryptor), std::move(transport), configuration_, serviceFactory_);
}

}
}
}
}
