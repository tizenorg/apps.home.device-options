Name:       device-options
Summary:    Device options application
Version:    0.0.1
Release:    1
Group:      Apps/home
License:    Apache License, Version 2.0
Source0:    device-options-%{version}.tar.gz
Source1:    org.tizen.poweroff-syspopup.manifest

BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(syspopup)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(efl-assist)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(capi-media-sound-manager)
BuildRequires:  pkgconfig(deviced)
BuildRequires:  pkgconfig(efl-assist)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  cmake
BuildRequires:  edje-bin
BuildRequires:  gettext-devel

Requires(post): /usr/bin/vconftool

%description
Device options popup application

%package -n org.tizen.poweroff-syspopup
Summary:    Power off popup
Group:      main

%description -n org.tizen.poweroff-syspopup
Power off system popup

%prep
%setup -q

%build

%if 0%{?simulator}
export CFLAGS+=" -DTIZEN_SIMULATOR"
%endif

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS+=" -DTIZEN_ENGINEER_MODE"
%endif

%if "%{?tizen_profile_name}" == "wearable"
export CFLAGS+=" -DDEVICE_OPTIONS_MICRO"
%endif

%if "%{?tizen_profile_name}" == "wearable"
export CFLAGS+=" -DDEVICE_OPTIONS_MICRO_3"
%endif

%if "%{?tizen_profile_name}" == "mobile"
export CFLAGS+=" -DDEVICE_OPTIONS_LITE"
%endif

cp %{SOURCE1} .

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/share/license

%files -n org.tizen.poweroff-syspopup
%manifest org.tizen.poweroff-syspopup.manifest
%defattr(-,root,root,-)
/usr/share/license/org.tizen.poweroff-syspopup
/etc/smack/accesses.d/org.tizen.poweroff-syspopup.efl
/usr/apps/org.tizen.poweroff-syspopup/bin/poweroff-syspopup
/usr/apps/org.tizen.poweroff-syspopup/res/edje/poweroff.edj
/usr/apps/org.tizen.poweroff-syspopup/res/table/poweroff-color.xml
/usr/apps/org.tizen.poweroff-syspopup/res/table/poweroff-font.xml
/usr/share/packages/org.tizen.poweroff-syspopup.xml
