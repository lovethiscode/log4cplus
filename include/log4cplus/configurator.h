// Module:  Log4CPLUS
// File:    configurator.h
// Created: 3/2003
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

/** @file */

#ifndef _CONFIGURATOR_HEADER_
#define _CONFIGURATOR_HEADER_

#include <log4cplus/config.hxx>
#include <log4cplus/appender.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/logger.h>
#include <log4cplus/helpers/logloguser.h>
#include <log4cplus/helpers/pointer.h>
#include <log4cplus/helpers/property.h>

#include <map>


namespace log4cplus
{

    /**
     * Provides configuration from an external file.  See configure() for
     * the expected format.
     * 
     * <em>All option values admit variable substitution.</em> For
     * example, if <code>userhome</code> environment property is set to
     * <code>/home/xyz</code> and the File option is set to the string
     * <code>${userhome}/test.log</code>, then File option will be
     * interpreted as the string <code>/home/xyz/test.log</code>.
     * 
     * The syntax of variable substitution is similar to that of UNIX
     * shells. The string between an opening <b>&quot;${&quot;</b> and
     * closing <b>&quot;}&quot;</b> is interpreted as a key. Its value is
     * searched in the environment properties.  The corresponding value replaces
     * the ${variableName} sequence.
     */
    class LOG4CPLUS_EXPORT PropertyConfigurator
        : protected log4cplus::helpers::LogLogUser
    {
    public:
        enum PCFlags
        {
            fRecursiveExpansion = 0x0001,
            fShadowEnvironment  = 0x0002,
            fAllowEmptyVars     = 0x0004
        };
        
        // ctor and dtor
        PropertyConfigurator(const log4cplus::tstring& propertyFile,
            Hierarchy& h = Logger::getDefaultHierarchy(), unsigned flags = 0);
        PropertyConfigurator(const log4cplus::helpers::Properties& props,
            Hierarchy& h = Logger::getDefaultHierarchy(), unsigned flags = 0);
        PropertyConfigurator(log4cplus::tistream& propertyStream,
            Hierarchy& h = Logger::getDefaultHierarchy(), unsigned flags = 0);
        virtual ~PropertyConfigurator();

        /**
         * This method eliminates the need to create a temporary
         * <code>PropertyConfigurator</code> to configure log4cplus.
         * It is equivalent to the following:<br>
         * <code>
         * PropertyConfigurator config("filename");
         * config.configure();
         * </code>
         */
        static void doConfigure(const log4cplus::tstring& configFilename,
            Hierarchy& h = Logger::getDefaultHierarchy(), unsigned flags = 0);

        /**
         * Read configuration from a file. <b>The existing configuration is
         * not cleared nor reset.</b> If you require a different behavior,
         * then call {@link Hierarchy::resetConfiguration
         * resetConfiguration} method before calling
         * <code>doConfigure</code>.
         *
         * The configuration file consists of statements in the format
         * <code>key=value</code>. The syntax of different configuration
         * elements are discussed below.
         *
         * <h3>Appender configuration</h3>
         *
         * Appender configuration syntax is:
         * <pre>
         * # For appender named <i>appenderName</i>, set its class.
         * # Note: The appender name can contain dots.
         * log4cplus.appender.appenderName=fully.qualified.name.of.appender.class
         *
         * # Set appender specific options.
         * log4cplus.appender.appenderName.option1=value1
         * ...
         * log4cplus.appender.appenderName.optionN=valueN
         * </pre>
         *
         * For each named appender you can configure its {@link Layout}. The
         * syntax for configuring an appender's layout is:
         * <pre>
         * log4cplus.appender.appenderName.layout=fully.qualified.name.of.layout.class
         * log4cplus.appender.appenderName.layout.option1=value1
         * ....
         * log4cplus.appender.appenderName.layout.optionN=valueN
         * </pre>
         *
         * <h3>Configuring loggers</h3>
         *
         * The syntax for configuring the root logger is:
         * <pre>
         * log4cplus.rootLogger=[LogLevel], appenderName, appenderName, ...
         * </pre>
         *
         * This syntax means that an optional <em>LogLevel value</em> can
         * be supplied followed by appender names separated by commas.
         *
         * The LogLevel value can consist of the string values FATAL,
         * ERROR, WARN, INFO, DEBUG or a <em>custom LogLevel</em> value.
         *
         * If a LogLevel value is specified, then the root LogLevel is set
         * to the corresponding LogLevel.  If no LogLevel value is specified,
         * then the root LogLevel remains untouched.
         *
         * The root logger can be assigned multiple appenders.
         *
         * Each <i>appenderName</i> (separated by commas) will be added to
         * the root logger. The named appender is defined using the
         * appender syntax defined above.
         *
         * For non-root loggers the syntax is almost the same:
         * <pre>
         * log4cplus.logger.logger_name=[LogLevel|INHERITED], appenderName, appenderName, ...
         * </pre>
         *
         * The meaning of the optional LogLevel value is discussed above
         * in relation to the root logger. In addition however, the value
         * INHERITED can be specified meaning that the named logger should
         * inherit its LogLevel from the logger hierarchy.
         *
         * By default loggers inherit their LogLevel from the
         * hierarchy.  However, if you set the LogLevel of a logger and
         * later decide that that logger should inherit its LogLevel, then
         * you should specify INHERITED as the value for the LogLevel value.
         *
         * Similar to the root logger syntax, each <i>appenderName</i>
         * (separated by commas) will be attached to the named logger.
         *
         * See the <a href="../../../../manual.html#additivity">appender
         * additivity rule</a> in the user manual for the meaning of the
         * <code>additivity</code> flag.
         *
         * The user can override any of the {@link
         * Hierarchy#disable} family of methods by setting the a key
         * "log4cplus.disableOverride" to <code>true</code> or any value other
         * than false. As in <pre>log4cplus.disableOverride=true </pre>
         *
         * <h3>Example</h3>
         *
         * An example configuration is given below.
         *
         * <pre>
         *
         * # Set options for appender named "A1".
         * # Appender "A1" will be a SyslogAppender
         * log4cplus.appender.A1=log4cplus::SyslogAppender
         *
         * # The syslog daemon resides on www.abc.net
         * log4cplus.appender.A1.SyslogHost=www.abc.net
         *
         * # A1's layout is a PatternLayout, using the conversion pattern
         * # <b>%r %-5p %c{2} %M.%L %x - %m\n</b>. Thus, the log output will
         * # include # the relative time since the start of the application in
         * # milliseconds, followed by the LogLevel of the log request,
         * # followed by the two rightmost components of the logger name,
         * # followed by the callers method name, followed by the line number,
         * # the nested disgnostic context and finally the message itself.
         * # Refer to the documentation of {@link PatternLayout} for further information
         * # on the syntax of the ConversionPattern key.
         * log4cplus.appender.A1.layout=log4cplus::PatternLayout
         * log4cplus.appender.A1.layout.ConversionPattern=%-4r %-5p %c{2} %M.%L %x - %m\n
         *
         * # Set options for appender named "A2"
         * # A2 should be a RollingFileAppender, with maximum file size of 10 MB
         * # using at most one backup file. A2's layout is TTCC, using the
         * # ISO8061 date format with context printing enabled.
         * log4cplus.appender.A2=log4cplus::RollingFileAppender
         * log4cplus.appender.A2.MaxFileSize=10MB
         * log4cplus.appender.A2.MaxBackupIndex=1
         * log4cplus.appender.A2.layout=log4cplus::TTCCLayout
         * log4cplus.appender.A2.layout.ContextPrinting=enabled
         * log4cplus.appender.A2.layout.DateFormat=ISO8601
         *
         * # Root logger set to DEBUG using the A2 appender defined above.
         * log4cplus.rootLogger=DEBUG, A2
         *
         * # Logger definitions:
         * # The SECURITY logger inherits is LogLevel from root. However, it's output
         * # will go to A1 appender defined above. It's additivity is non-cumulative.
         * log4cplus.logger.SECURITY=INHERIT, A1
         * log4cplus.additivity.SECURITY=false
         *
         * # Only warnings or above will be logged for the logger "SECURITY.access".
         * # Output will go to A1.
         * log4cplus.logger.SECURITY.access=WARN
         *
         *
         * # The logger "class.of.the.day" inherits its LogLevel from the
         * # logger hierarchy.  Output will go to the appender's of the root
         * # logger, A2 in this case.
         * log4cplus.logger.class.of.the.day=INHERIT
         * </pre>
         *
         * Refer to the <b>setOption</b> method in each Appender and
         * Layout for class specific options.
         *
         * Use the <code>#</code> character at the beginning of a line for comments.
         */
        virtual void configure();

    protected:
      // Methods
        void init();  // called by the ctor
        void reconfigure();
        void replaceEnvironVariables();
        void configureLoggers();
        void configureLogger(log4cplus::Logger logger, const log4cplus::tstring& config);
        void configureAppenders();
        void configureAdditivity();
        
        virtual Logger getLogger(const log4cplus::tstring& name);
        virtual void addAppender(Logger &logger, log4cplus::SharedAppenderPtr& appender);

      // Types
        typedef std::map<log4cplus::tstring, log4cplus::SharedAppenderPtr> AppenderMap;

      // Data
        Hierarchy& h;
        log4cplus::tstring propertyFilename;
        log4cplus::helpers::Properties properties; 
        AppenderMap appenders;
        unsigned flags;
        
    private:
      // Disable copy
        PropertyConfigurator(const PropertyConfigurator&);
        PropertyConfigurator& operator=(PropertyConfigurator&);
    };



    /**
     * Use this class to quickly configure the package. For file based 
     * configuration see PropertyConfigurator.
     */
    class LOG4CPLUS_EXPORT BasicConfigurator : public PropertyConfigurator {
    public:
      // ctor and dtor
        BasicConfigurator(Hierarchy& h = Logger::getDefaultHierarchy());
        virtual ~BasicConfigurator();

        /**
         * This method eliminates the need to create a temporary
         * <code>BasicConfigurator</code> object to configure log4cplus.
         * It is equivalent to the following:<br>
         * <code>
         * BasicConfigurator config();
         * config.configure();
         * </code>
         */
        static void doConfigure(Hierarchy& h = Logger::getDefaultHierarchy());
        
    private:
      // Disable copy
        BasicConfigurator(const BasicConfigurator&);
        BasicConfigurator& operator=(BasicConfigurator&);
    };
   

#if !defined(LOG4CPLUS_SINGLE_THREADED)
    // Forward Declarations
    class ConfigurationWatchDogThread;
    
    
    class LOG4CPLUS_EXPORT ConfigureAndWatchThread {
    public:
      // ctor and dtor
        ConfigureAndWatchThread(const log4cplus::tstring& propertyFile,
                                unsigned int millis = 60 * 1000);
        virtual ~ConfigureAndWatchThread();

    private:
      // Disallow copying of instances of this class
       ConfigureAndWatchThread(const ConfigureAndWatchThread&);
       ConfigureAndWatchThread& operator=(const ConfigureAndWatchThread&);
       
      // Data
       ConfigurationWatchDogThread * watchDogThread;
    };
#endif

} // end namespace log4cplus

#endif // _CONFIGURATOR_HEADER_
